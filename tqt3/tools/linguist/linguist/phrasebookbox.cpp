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

/*  TRANSLATOR PhraseBookBox

  Go to Phrase > Edit Phrase Book...  The dialog that pops up is a
  PhraseBookBox.
*/

#include "phrasebookbox.h"
#include "phraselv.h"

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqlayout.h>
#include <ntqlineedit.h>
#include <ntqmessagebox.h>
#include <ntqpushbutton.h>
#include <ntqwhatsthis.h>

PhraseBookBox::PhraseBookBox( const TQString& filename,
			      const PhraseBook& phraseBook, TQWidget *parent,
			      const char *name, bool modal )
    : TQDialog( parent, name, modal ), fn( filename ), pb( phraseBook )
{
    TQGridLayout *gl = new TQGridLayout( this, 4, 3, 11, 11,
				       "phrase book outer layout" );
    TQVBoxLayout *bl = new TQVBoxLayout( 6, "phrase book button layout" );

    sourceLed = new TQLineEdit( this, "source line edit" );
    TQLabel *source = new TQLabel( sourceLed, tr("S&ource phrase:"), this,
				 "source label" );
    targetLed = new TQLineEdit( this, "target line edit" );
    TQLabel *target = new TQLabel( targetLed, tr("&Translation:"), this,
				 "target label" );
    definitionLed = new TQLineEdit( this, "definition line edit" );
    TQLabel *definition = new TQLabel( definitionLed, tr("&Definition:"), this,
				     "target label" );
    lv = new PhraseLV( this, "phrase book list view" );

    newBut = new TQPushButton( tr("&New Phrase"), this );
    newBut->setDefault( true );

    removeBut = new TQPushButton( tr("&Remove Phrase"), this );
    removeBut->setEnabled( false );
    TQPushButton *saveBut = new TQPushButton( tr("&Save"), this );
    TQPushButton *closeBut = new TQPushButton( tr("Close"), this );

    gl->addWidget( source, 0, 0 );
    gl->addWidget( sourceLed, 0, 1 );
    gl->addWidget( target, 1, 0 );
    gl->addWidget( targetLed, 1, 1 );
    gl->addWidget( definition, 2, 0 );
    gl->addWidget( definitionLed, 2, 1 );
    gl->addMultiCellWidget( lv, 3, 3, 0, 1 );
    gl->addMultiCell( bl, 0, 3, 2, 2 );

    bl->addWidget( newBut );
    bl->addWidget( removeBut );
    bl->addWidget( saveBut );
    bl->addWidget( closeBut );
    bl->addStretch( 1 );

    connect( sourceLed, TQ_SIGNAL(textChanged(const TQString&)),
	     this, TQ_SLOT(sourceChanged(const TQString&)) );
    connect( targetLed, TQ_SIGNAL(textChanged(const TQString&)),
	     this, TQ_SLOT(targetChanged(const TQString&)) );
    connect( definitionLed, TQ_SIGNAL(textChanged(const TQString&)),
	     this, TQ_SLOT(definitionChanged(const TQString&)) );
    connect( lv, TQ_SIGNAL(selectionChanged(TQListViewItem *)),
	     this, TQ_SLOT(selectionChanged(TQListViewItem *)) );
    connect( newBut, TQ_SIGNAL(clicked()), this, TQ_SLOT(newPhrase()) );
    connect( removeBut, TQ_SIGNAL(clicked()), this, TQ_SLOT(removePhrase()) );
    connect( saveBut, TQ_SIGNAL(clicked()), this, TQ_SLOT(save()) );
    connect( closeBut, TQ_SIGNAL(clicked()), this, TQ_SLOT(accept()) );

    PhraseBook::ConstIterator it;
    for ( it = phraseBook.begin(); it != phraseBook.end(); ++it )
	(void) new PhraseLVI( lv, (*it) );
    enableDisable();

    TQWhatsThis::add( this, tr("This window allows you to add, modify, or delete"
			      " phrases in a phrase book.") );
    TQWhatsThis::add( sourceLed, tr("This is the phrase in the source"
				   " language.") );
    TQWhatsThis::add( targetLed, tr("This is the phrase in the target language"
				   " corresponding to the source phrase.") );
    TQWhatsThis::add( definitionLed, tr("This is a definition for the source"
				       " phrase.") );
    TQWhatsThis::add( newBut, tr("Click here to add the phrase to the phrase"
				" book.") );
    TQWhatsThis::add( removeBut, tr("Click here to remove the phrase from the"
				   " phrase book.") );
    TQWhatsThis::add( saveBut, tr("Click here to save the changes made.") );
    TQWhatsThis::add( closeBut, tr("Click here to close this window.") );
}

void PhraseBookBox::keyPressEvent( TQKeyEvent *ev )
{
    if ( ev->key() == Key_Down || ev->key() == Key_Up ||
	 ev->key() == Key_Next || ev->key() == Key_Prior )
	TQApplication::sendEvent( lv,
		new TQKeyEvent(ev->type(), ev->key(), ev->ascii(), ev->state(),
			      ev->text(), ev->isAutoRepeat(), ev->count()) );
    else
	TQDialog::keyPressEvent( ev );
}

void PhraseBookBox::newPhrase()
{
    Phrase ph;
    ph.setSource( NewPhrase );
    TQListViewItem *item = new PhraseLVI( lv, ph );
    selectItem( item );
}

void PhraseBookBox::removePhrase()
{
    TQListViewItem *item = lv->currentItem();
    TQListViewItem *next = item->itemBelow() != 0 ? item->itemBelow()
			  : item->itemAbove();
    delete item;
    if ( next != 0 )
	selectItem( next );
    enableDisable();
}

void PhraseBookBox::save()
{
    pb.clear();
    TQListViewItem *item = lv->firstChild();
    while ( item != 0 ) {
	if ( !item->text(PhraseLVI::SourceTextShown).isEmpty() &&
	     item->text(PhraseLVI::SourceTextShown) != NewPhrase )
	    pb.append( Phrase(((PhraseLVI *) item)->phrase()) );
	item = item->nextSibling();
    }
    if ( !pb.save( fn ) )
	TQMessageBox::warning( this, tr("TQt Linguist"),
			      tr("Cannot save phrase book '%1'.").arg(fn) );
}

void PhraseBookBox::sourceChanged( const TQString& source )
{
    if ( lv->currentItem() != 0 ) {
	lv->currentItem()->setText( PhraseLVI::SourceTextShown,
				    source.stripWhiteSpace() );
	lv->currentItem()->setText( PhraseLVI::SourceTextOriginal, source );
	lv->sort();
	lv->ensureItemVisible( lv->currentItem() );
    }
}

void PhraseBookBox::targetChanged( const TQString& target )
{
    if ( lv->currentItem() != 0 ) {
	lv->currentItem()->setText( PhraseLVI::TargetTextShown,
				    target.stripWhiteSpace() );
	lv->currentItem()->setText( PhraseLVI::TargetTextOriginal, target );
	lv->sort();
	lv->ensureItemVisible( lv->currentItem() );
    }
}

void PhraseBookBox::definitionChanged( const TQString& definition )
{
    if ( lv->currentItem() != 0 ) {
	lv->currentItem()->setText( PhraseLVI::DefinitionText, definition );
	lv->sort();
	lv->ensureItemVisible( lv->currentItem() );
    }
}

void PhraseBookBox::selectionChanged( TQListViewItem * /* item */ )
{
    enableDisable();
}

void PhraseBookBox::selectItem( TQListViewItem *item )
{
    lv->setSelected( item, true );
    lv->ensureItemVisible( item );
}

void PhraseBookBox::enableDisable()
{
    TQListViewItem *item = lv->currentItem();

    sourceLed->blockSignals( true );
    targetLed->blockSignals( true );
    definitionLed->blockSignals( true );

    if ( item == 0 ) {
	sourceLed->setText( TQString::null );
	targetLed->setText( TQString::null );
	definitionLed->setText( TQString::null );
    } else {
	sourceLed->setText( item->text(0) );
	targetLed->setText( item->text(1) );
	definitionLed->setText( item->text(2) );
    }
    sourceLed->setEnabled( item != 0 );
    targetLed->setEnabled( item != 0 );
    definitionLed->setEnabled( item != 0 );
    removeBut->setEnabled( item != 0 );

    sourceLed->blockSignals( false );
    targetLed->blockSignals( false );
    definitionLed->blockSignals( false );

    TQLineEdit *led = ( sourceLed->text() == NewPhrase ? sourceLed : targetLed );
    led->setFocus();
    led->selectAll();
}
