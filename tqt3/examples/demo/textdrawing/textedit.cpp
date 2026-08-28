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
    : TQMainWindow( parent, name, 0 )
{
    setupFileActions();
    setupEditActions();
    setupTextActions();

    tabWidget = new TQTabWidget( this );
    connect( tabWidget, TQ_SIGNAL( currentChanged( TQWidget * ) ),
	     this, TQ_SLOT( editorChanged( TQWidget * ) ) );
    setCentralWidget( tabWidget );
}

void TextEdit::setupFileActions()
{
    TQToolBar *tb = new TQToolBar( this );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "&File" ), menu );

    TQAction *a;
    a = new TQAction( tr( "New" ), TQPixmap( "textdrawing/filenew.png" ), tr( "&New..." ), CTRL + Key_N, this, "fileNew" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileNew() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Open" ), TQPixmap( "textdrawing/fileopen.png" ), tr( "&Open..." ), CTRL + Key_O, this, "fileOpen" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileOpen() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( tr( "Save" ), TQPixmap( "textdrawing/filesave.png" ), tr( "&Save..." ), CTRL + Key_S, this, "fileSave" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSave() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Save As" ), TQPixmap(), tr( "Save &As..." ), 0, this, "fileSaveAs" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileSaveAs() ) );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( tr( "Print" ), TQPixmap( "textdrawing/print.png" ), tr( "&Print..." ), CTRL + Key_P, this, "filePrint" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( filePrint() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Close" ), TQPixmap(), tr( "&Close" ), 0, this, "fileClose" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( fileClose() ) );
    a->addTo( menu );
}

void TextEdit::setupEditActions()
{
    TQToolBar *tb = new TQToolBar( this );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "&Edit" ), menu );

    TQAction *a;
    a = new TQAction( tr( "Undo" ), TQPixmap( "textdrawing/undo.png" ), tr( "&Undo" ), CTRL + Key_Z, this, "editUndo" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editUndo() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Redo" ), TQPixmap( "textdrawing/redo.png" ), tr( "&Redo" ), CTRL + Key_Y, this, "editRedo" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editRedo() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new TQAction( tr( "Cut" ), TQPixmap( "textdrawing/editcut.png" ), tr( "&Cut" ), CTRL + Key_X, this, "editCut" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCut() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Copy" ), TQPixmap( "textdrawing/editcopy.png" ), tr( "C&opy" ), CTRL + Key_C, this, "editCopy" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editCopy() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new TQAction( tr( "Paste" ), TQPixmap( "textdrawing/editpaste.png" ), tr( "&Paste" ), CTRL + Key_V, this, "editPaste" );
    connect( a, TQ_SIGNAL( activated() ), this, TQ_SLOT( editPaste() ) );
    a->addTo( tb );
    a->addTo( menu );
}

void TextEdit::setupTextActions()
{
    TQToolBar *tb = new TQToolBar( this );
    TQPopupMenu *menu = new TQPopupMenu( this );
    menuBar()->insertItem( tr( "For&mat" ), menu );

    comboStyle = new TQComboBox( false, tb );
    comboStyle->insertItem( tr("Standard") );
    comboStyle->insertItem( tr("Bullet List (Disc)") );
    comboStyle->insertItem( tr("Bullet List (Circle)") );
    comboStyle->insertItem( tr("Bullet List (Square)") );
    comboStyle->insertItem( tr("Ordered List (Decimal)") );
    comboStyle->insertItem( tr("Ordered List (Alpha lower)") );
    comboStyle->insertItem( tr("Ordered List (Alpha upper)") );
    connect( comboStyle, TQ_SIGNAL( activated( int ) ),
	     this, TQ_SLOT( textStyle( int ) ) );

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

    actionTextBold = new TQAction( tr( "Bold" ), TQPixmap( "textdrawing/textbold.png" ), tr( "&Bold" ), CTRL + Key_B, this, "textBold" );
    connect( actionTextBold, TQ_SIGNAL( activated() ), this, TQ_SLOT( textBold() ) );
    actionTextBold->addTo( tb );
    actionTextBold->addTo( menu );
    actionTextBold->setToggleAction( true );
    actionTextItalic = new TQAction( tr( "Italic" ), TQPixmap( "textdrawing/textitalic.png" ), tr( "&Italic" ), CTRL + Key_I, this, "textItalic" );
    connect( actionTextItalic, TQ_SIGNAL( activated() ), this, TQ_SLOT( textItalic() ) );
    actionTextItalic->addTo( tb );
    actionTextItalic->addTo( menu );
    actionTextItalic->setToggleAction( true );
    actionTextUnderline = new TQAction( tr( "Underline" ), TQPixmap( "textdrawing/textunderline.png" ), tr( "&Underline" ), CTRL + Key_U, this, "textUnderline" );
    connect( actionTextUnderline, TQ_SIGNAL( activated() ), this, TQ_SLOT( textUnderline() ) );
    actionTextUnderline->addTo( tb );
    actionTextUnderline->addTo( menu );
    actionTextUnderline->setToggleAction( true );
    menu->insertSeparator();

    TQActionGroup *grp = new TQActionGroup( this );
    grp->setExclusive( true );
    connect( grp, TQ_SIGNAL( selected( TQAction* ) ), this, TQ_SLOT( textAlign( TQAction* ) ) );

    actionAlignLeft = new TQAction( tr( "Left" ), TQPixmap( "textdrawing/textleft.png" ), tr( "&Left" ), CTRL + Key_L, grp, "textLeft" );
    actionAlignLeft->addTo( tb );
    actionAlignLeft->addTo( menu );
    actionAlignLeft->setToggleAction( true );
    actionAlignCenter = new TQAction( tr( "Center" ), TQPixmap( "textdrawing/textcenter.png" ), tr( "C&enter" ), CTRL + Key_M, grp, "textCenter" );
    actionAlignCenter->addTo( tb );
    actionAlignCenter->addTo( menu );
    actionAlignCenter->setToggleAction( true );
    actionAlignRight = new TQAction( tr( "Right" ), TQPixmap( "textdrawing/textright.png" ), tr( "&Right" ), CTRL + Key_R, grp, "textRight" );
    actionAlignRight->addTo( tb );
    actionAlignRight->addTo( menu );
    actionAlignRight->setToggleAction( true );
    actionAlignJustify = new TQAction( tr( "Justify" ), TQPixmap( "textdrawing/textjustify.png" ), tr( "&Justify" ), CTRL + Key_J, grp, "textjustify" );
    actionAlignJustify->addTo( tb );
    actionAlignJustify->addTo( menu );
    actionAlignJustify->setToggleAction( true );

    menu->insertSeparator();

    TQPixmap pix( 16, 16 );
    pix.fill( black );
    actionTextColor = new TQAction( tr( "Color" ), pix, tr( "&Color..." ), 0, this, "textColor" );
    connect( actionTextColor, TQ_SIGNAL( activated() ), this, TQ_SLOT( textColor() ) );
    actionTextColor->addTo( tb );
    actionTextColor->addTo( menu );
}

void TextEdit::load( const TQString &f )
{
    if ( !TQFile::exists( f ) )
	return;
    TQTextEdit *edit = new TQTextEdit( tabWidget );
    doConnections( edit );
    tabWidget->addTab( edit, TQFileInfo( f ).fileName() );

    TQFile fl( f );
    fl.open( IO_ReadOnly );
    TQByteArray array = fl.readAll();
    array.resize( array.size() +1 );
    array[ (int)array.size() - 1 ] = '\0';
    TQString text = ( f.find( "bidi.txt" ) != -1 ? TQString::fromUtf8( array.data() ) : TQString::fromLatin1( array.data() ) );
    edit->setText( text );

    edit->viewport()->setFocus();
    edit->setTextFormat( TQt::RichText );
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
    TQPrinter printer;
    printer.setFullPage(true);
    TQPaintDeviceMetrics screen( this );
    printer.setResolution( screen.logicalDpiY() );
    if ( printer.setup( this ) ) {
	TQPainter p( &printer );
	TQPaintDeviceMetrics metrics( p.device() );
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	TQRect body( margin * dpix / 72, margin * dpiy / 72,
		    metrics.width() - margin * dpix / 72 * 2,
		    metrics.height() - margin * dpiy / 72 * 2 );
	TQFont font( "times", 10 );
	TQSimpleRichText richText( currentEditor()->text(), font, currentEditor()->context(), currentEditor()->styleSheet(),
				  currentEditor()->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	TQRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( TQString::number( page ) ),
			view.bottom() + p.fontMetrics().ascent() + 5, TQString::number( page ) );
	    if ( view.top()  >= richText.height() )
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

void TextEdit::textStyle( int i )
{
    if ( !currentEditor() )
	return;
    if ( i == 0 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayBlock, TQStyleSheetItem::ListDisc );
    else if ( i == 1 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListDisc );
    else if ( i == 2 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListCircle );
    else if ( i == 3 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListSquare );
    else if ( i == 4 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListDecimal );
    else if ( i == 5 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListLowerAlpha );
    else if ( i == 6 )
	currentEditor()->setParagType( TQStyleSheetItem::DisplayListItem, TQStyleSheetItem::ListUpperAlpha );
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
    pix.fill( col );
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
    fontChanged( currentEditor()->font() );
    colorChanged( currentEditor()->color() );
    alignmentChanged( currentEditor()->alignment() );
}
