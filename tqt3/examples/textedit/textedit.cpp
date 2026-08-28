/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "textedit.h"

#include <ntqtextedit.h>
#include <ntqaction.h>
#include <ntqmenubar.h>
#include <ntqpopupmenu.h>
#include <ntqtoolbar.h>
#include <ntqtabwidget.h>
#include <ntqapplication.h>
#include <ntqfontdatabase.h>
#include <ntqcombobox.h>
#include <ntqlineedit.h>
#include <ntqfileinfo.h>
#include <ntqfile.h>
#include <ntqfiledialog.h>
#include <ntqprinter.h>
#include <ntqpaintdevicemetrics.h>
#include <ntqsimplerichtext.h>
#include <ntqcolordialog.h>
#include <ntqpainter.h>

TextEdit::TextEdit( TQWidget *parent, const char *name )
    : TQMainWindow( parent, name )
{
    setupFileActions();
    setupEditActions();
    setupTextActions();

    tabWidget = new TQTabWidget( this );
    connect( tabWidget, TQ_SIGNAL( currentChanged( TQWidget * ) ),
	     this, TQ_SLOT( editorChanged( TQWidget * ) ) );
    setCentralWidget( tabWidget );

    if ( tqApp->argc() == 1 ) {
	load( "example.html" );
    } else {
	for ( int i = 1; i < tqApp->argc(); ++i )
	    load( tqApp->argv()[ i ] );
    }
}

void TextEdit::setupFileActions()
{
    TQToolBar *tb = new TQToolBar( this );
    tb->setLabel( "File Actions" );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "&File" ), menu );

    TQAction *a;
    a = new TQAction( TQPixmap::fromMimeSource( "filenew.xpm" ), tr( "&New..." ), CTRL + Key_N, this, "fileNew" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNew() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( TQPixmap::fromMimeSource( "fileopen.xpm" ), tr( "&Open..." ), CTRL + Key_O, this, "fileOpen" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileOpen() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( TQPixmap::fromMimeSource( "filesave.xpm" ), tr( "&Save..." ), CTRL + Key_S, this, "fileSave" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSave() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Save &As..." ), 0, this, "fileSaveAs" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSaveAs() ) );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( TQPixmap::fromMimeSource( "fileprint.xpm" ), tr( "&Print..." ), CTRL + Key_P, this, "filePrint" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrint() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "&Close" ), 0, this, "fileClose" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileClose() ) );
    a->addTo( menu );
    a = new TQAction( tr( "E&xit" ), 0, this, "fileExit" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileExit() ) );
    a->addTo( menu );
}

void TextEdit::setupEditActions()
{
    TQToolBar *tb = new TQToolBar( this );
    tb->setLabel( "Edit Actions" );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "&Edit" ), menu );

    TQAction *a;
    a = new TQAction( TQPixmap::fromMimeSource( "editundo.xpm" ), tr( "&Undo" ), CTRL + Key_Z, this, "editUndo" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editUndo() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( TQPixmap::fromMimeSource( "editredo.xpm" ), tr( "&Redo" ), CTRL + Key_Y, this, "editRedo" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editRedo() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( TQPixmap::fromMimeSource( "editcopy.xpm" ), tr( "&Copy" ), CTRL + Key_C, this, "editCopy" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCopy() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( TQPixmap::fromMimeSource( "editcut.xpm" ), tr( "Cu&t" ), CTRL + Key_X, this, "editCut" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCut() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( TQPixmap::fromMimeSource( "editpaste.xpm" ), tr( "&Paste" ), CTRL + Key_V, this, "editPaste" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editPaste() ) );
    a->addTo( tb );
    a->addTo( menu );
}

void TextEdit::setupTextActions()
{
    TQToolBar *tb = new TQToolBar( this );
    tb->setLabel( "Format Actions" );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "F&ormat" ), menu );

    comboFont = new TQComboBox( true, tb );
    TQFontDatabase db;
    comboFont->insertStringList( db.families() );
    connect( comboFont, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT( textFamily( const TQString & ) ) );
    comboFont->lineEdit()->setText( TQApplication::font().family() );

    comboSize = new TQComboBox( true, tb );
    TQValueList<int> sizes = db.standardSizes();
    TQValueList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
	comboSize->insertItem( TQString::number( *it ) );
    connect( comboSize, TQ_SIGNAL( activated( const TQString & ) ),
	     this, TQ_SLOT( textSize( const TQString & ) ) );
    comboSize->lineEdit()->setText( TQString::number( TQApplication::font().pointSize() ) );

    actionTextBold = new TQAction( TQPixmap::fromMimeSource( "textbold.xpm" ), tr( "&Bold" ), CTRL + Key_B, this, "textBold" );
    connect( actionTextBold, TQ_SIGNAL( activated() ), this, TQ_SLOT( textBold() ) );
    actionTextBold->addTo( tb );
    actionTextBold->addTo( menu );
    actionTextBold->setToggleAction( true );
    actionTextItalic = new TQAction( TQPixmap::fromMimeSource( "textitalic.xpm" ), tr( "&Italic" ), CTRL + Key_I, this, "textItalic" );
    connect( actionTextItalic, TQ_SIGNAL( activated() ), this, TQ_SLOT( textItalic() ) );
    actionTextItalic->addTo( tb );
    actionTextItalic->addTo( menu );
    actionTextItalic->setToggleAction( true );
    actionTextUnderline = new TQAction( TQPixmap::fromMimeSource( "textunder.xpm" ), tr( "&Underline" ), CTRL + Key_U, this, "textUnderline" );
    connect( actionTextUnderline, TQ_SIGNAL( activated() ), this, TQ_SLOT( textUnderline() ) );
    actionTextUnderline->addTo( tb );
    actionTextUnderline->addTo( menu );
    actionTextUnderline->setToggleAction( true );
    menu->insertSeparator();

    TQActionGroup *grp = new TQActionGroup( this );
    connect( grp, TQ_SIGNAL( selected( TQAction* ) ), this, TQ_SLOT( textAlign( TQAction* ) ) );

    actionAlignLeft = new TQAction( TQPixmap::fromMimeSource( "textleft.xpm" ), tr( "&Left" ), CTRL + Key_L, grp, "textLeft" );
    actionAlignLeft->setToggleAction( true );
    actionAlignCenter = new TQAction( TQPixmap::fromMimeSource( "textcenter.xpm" ), tr( "C&enter" ), CTRL + Key_E, grp, "textCenter" );
    actionAlignCenter->setToggleAction( true );
    actionAlignRight = new TQAction( TQPixmap::fromMimeSource( "textright.xpm" ), tr( "&Right" ), CTRL + Key_R, grp, "textRight" );
    actionAlignRight->setToggleAction( true );
    actionAlignJustify = new TQAction( TQPixmap::fromMimeSource( "textjustify.xpm" ), tr( "&Justify" ), CTRL + Key_J, grp, "textjustify" );
    actionAlignJustify->setToggleAction( true );

    grp->addTo( tb );
    grp->addTo( menu );

    menu->insertSeparator();

    TQPixmap pix( 16, 16 );
    pix.fill( black );
    actionTextColor = new TQAction( pix, tr( "&Color..." ), 0, this, "textColor" );
    connect( actionTextColor, TQ_SIGNAL( activated() ), this, TQ_SLOT( textColor() ) );
    actionTextColor->addTo( tb );
    actionTextColor->addTo( menu );
}

void TextEdit::load( const TQString &f )
{
    if ( !TQFile::exists( f ) )
	return;
    TQTextEdit *edit = new TQTextEdit( tabWidget );
    edit->setTextFormat( RichText );
    doConnections( edit );
    tabWidget->addTab( edit, TQFileInfo( f ).fileName() );
    TQFile file( f );
    if ( !file.open( IO_ReadOnly ) )
	return;
    TQTextStream ts( &file );
    TQString txt = ts.read();
    if ( !TQStyleSheet::mightBeRichText( txt ) )
	txt = TQStyleSheet::convertFromPlainText( txt, TQStyleSheetItem::WhiteSpacePre );
    edit->setText( txt );
    tabWidget->showPage( edit );
    edit->viewport()->setFocus();
    filenames.replace( edit, f );
}

TQTextEdit *TextEdit::currentEditor() const
{
    if ( tabWidget->currentPage() &&
	 tabWidget->currentPage()->inherits( "TQTextEdit" ) )
	return (TQTextEdit*)tabWidget->currentPage();
    return 0;
}

void TextEdit::doConnections( TQTextEdit *e )
{
    connect( e, TQ_SIGNAL( currentFontChanged( const TQFont & ) ),
	     this, TQ_SLOT( fontChanged( const TQFont & ) ) );
    connect( e, TQ_SIGNAL( currentColorChanged( const TQColor & ) ),
	     this, TQ_SLOT( colorChanged( const TQColor & ) ) );
    connect( e, TQ_SIGNAL( currentAlignmentChanged( int ) ),
	     this, TQ_SLOT( alignmentChanged( int ) ) );
}

void TextEdit::fileNew()
{
    TQTextEdit *edit = new TQTextEdit( tabWidget );
    edit->setTextFormat( RichText );
    doConnections( edit );
    tabWidget->addTab( edit, tr( "noname" ) );
    tabWidget->showPage( edit );
    edit->viewport()->setFocus();
}

void TextEdit::fileOpen()
{
    TQString fn = TQFileDialog::getOpenFileName( TQString::null, tr( "HTML-Files (*.htm *.html);;All Files (*)" ), this );
    if ( !fn.isEmpty() )
	load( fn );
}

void TextEdit::fileSave()
{
    if ( !currentEditor() )
	return;
    TQString fn;
    if ( filenames.find( currentEditor() ) == filenames.end() ) {
	fileSaveAs();
    } else {
	TQFile file( *filenames.find( currentEditor() ) );
	if ( !file.open( IO_WriteOnly ) )
	    return;
	TQTextStream ts( &file );
	ts << currentEditor()->text();
    }
}

void TextEdit::fileSaveAs()
{
    if ( !currentEditor() )
	return;
    TQString fn = TQFileDialog::getSaveFileName( TQString::null, tr( "HTML-Files (*.htm *.html);;All Files (*)" ), this );
    if ( !fn.isEmpty() ) {
	filenames.replace( currentEditor(), fn );
	fileSave();
	tabWidget->setTabLabel( currentEditor(), TQFileInfo( fn ).fileName() );
    }
}

void TextEdit::filePrint()
{
    if ( !currentEditor() )
	return;
#ifndef TQT_NO_PRINTER
    TQPrinter printer( TQPrinter::HighResolution );
    printer.setFullPage(true);
    if ( printer.setup( this ) ) {
	TQPainter p( &printer );
	// Check that there is a valid device to print to.
	if ( !p.device() ) return;
	TQPaintDeviceMetrics metrics( p.device() );
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	TQRect view( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	TQFont font( currentEditor()->TQWidget::font() );
 	font.setPointSize( 10 ); // we define 10pt to be a nice base size for printing

	TQSimpleRichText richText( currentEditor()->text(), font,
				  currentEditor()->context(),
				  currentEditor()->styleSheet(),
				  currentEditor()->mimeSourceFactory(),
				  view.height() );
	richText.setWidth( &p, view.width() );
	int page = 1;
	do {
	    richText.draw( &p, margin, margin, view, colorGroup() );
	    view.moveBy( 0, view.height() );
	    p.translate( 0 , -view.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number( page ) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number( page ) );
	    if ( view.top() - margin >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (true);
    }
#endif
}

void TextEdit::fileClose()
{
    delete currentEditor();
    if ( currentEditor() )
	currentEditor()->viewport()->setFocus();
}

void TextEdit::fileExit()
{
    tqApp->quit();
}

void TextEdit::editUndo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->undo();
}

void TextEdit::editRedo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->redo();
}

void TextEdit::editCut()
{
    if ( !currentEditor() )
	return;
    currentEditor()->cut();
}

void TextEdit::editCopy()
{
    if ( !currentEditor() )
	return;
    currentEditor()->copy();
}

void TextEdit::editPaste()
{
    if ( !currentEditor() )
	return;
    currentEditor()->paste();
}

void TextEdit::textBold()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setBold( actionTextBold->isOn() );
}

void TextEdit::textUnderline()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setUnderline( actionTextUnderline->isOn() );
}

void TextEdit::textItalic()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setItalic( actionTextItalic->isOn() );
}

void TextEdit::textFamily( const TQString &f )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setFamily( f );
    currentEditor()->viewport()->setFocus();
}

void TextEdit::textSize( const TQString &p )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setPointSize( p.toInt() );
    currentEditor()->viewport()->setFocus();
}

void TextEdit::textColor()
{
    if ( !currentEditor() )
	return;
    TQColor col = TQColorDialog::getColor( currentEditor()->color(), this );
    if ( !col.isValid() )
	return;
    currentEditor()->setColor( col );
    TQPixmap pix( 16, 16 );
    pix.fill( black );
    actionTextColor->setIconSet( pix );
}

void TextEdit::textAlign( TQAction *a )
{
    if ( !currentEditor() )
	return;
    if ( a == actionAlignLeft )
	currentEditor()->setAlignment( AlignLeft );
    else if ( a == actionAlignCenter )
	currentEditor()->setAlignment( AlignHCenter );
    else if ( a == actionAlignRight )
	currentEditor()->setAlignment( AlignRight );
    else if ( a == actionAlignJustify )
	currentEditor()->setAlignment( AlignJustify );
}

void TextEdit::fontChanged( const TQFont &f )
{
    comboFont->lineEdit()->setText( f.family() );
    comboSize->lineEdit()->setText( TQString::number( f.pointSize() ) );
    actionTextBold->setOn( f.bold() );
    actionTextItalic->setOn( f.italic() );
    actionTextUnderline->setOn( f.underline() );
}

void TextEdit::colorChanged( const TQColor &c )
{
    TQPixmap pix( 16, 16 );
    pix.fill( c );
    actionTextColor->setIconSet( pix );
}

void TextEdit::alignmentChanged( int a )
{
    if ( ( a == AlignAuto ) || ( a & AlignLeft ))
	actionAlignLeft->setOn( true );
    else if ( ( a & AlignHCenter ) )
	actionAlignCenter->setOn( true );
    else if ( ( a & AlignRight ) )
	actionAlignRight->setOn( true );
    else if ( ( a & AlignJustify ) )
	actionAlignJustify->setOn( true );
}

void TextEdit::editorChanged( TQWidget * )
{
    if ( !currentEditor() )
	return;
    fontChanged( currentEditor()->currentFont() );
    colorChanged( currentEditor()->color() );
    alignmentChanged( currentEditor()->alignment() );
}
