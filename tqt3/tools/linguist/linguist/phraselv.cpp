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

/*  TRANSLATOR PhraseLV

  The phrase list in the right panel of the main window (with Source phrase,
  Target phrase, and Definition in its header) is a PhraseLV object.
*/

#include "phraselv.h"

#include <ntqregexp.h>
#include <ntqwhatsthis.h>
#include <ntqheader.h>

class WhatPhrase : public TQWhatsThis
{
public:
    WhatPhrase( PhraseLV *w );

    virtual TQString text( const TQPoint& p );

private:
    PhraseLV *parent;
};

WhatPhrase::WhatPhrase( PhraseLV *w )
    : TQWhatsThis( w )
{
    parent = w;
}

TQString WhatPhrase::text( const TQPoint& p )
{
    TQListViewItem *item = parent->itemAt( p );
    if ( item == 0 )
	return PhraseLV::tr( "This is a list of phrase entries relevant to the"
		  " source text.  Each phrase is supplemented with a suggested"
		  " translation and a definition." );
    else
	return TQString( PhraseLV::tr("<p><u>%1:</u>&nbsp;&nbsp;%2</p>"
				     "<p><u>%3:</u>&nbsp;&nbsp;%4</p>"
				     "<p><u>%5:</u>&nbsp;&nbsp;%6</p>") )
	       .arg( parent->columnText(PhraseLVI::SourceTextShown) )
	       .arg( item->text(PhraseLVI::SourceTextShown) )
	       .arg( parent->columnText(PhraseLVI::TargetTextShown) )
	       .arg( item->text(PhraseLVI::TargetTextShown) )
	       .arg( parent->columnText(PhraseLVI::DefinitionText) )
	       .arg( item->text(PhraseLVI::DefinitionText) );
}

PhraseLVI::PhraseLVI( PhraseLV *parent, const Phrase& phrase, int accelKey )
    : TQListViewItem( parent ),
      akey( accelKey )
{
    setPhrase( phrase );
}

TQString PhraseLVI::key( int column, bool ascending ) const
{
    if ( column == SourceTextShown ) {
	if ( sourceTextKey.isEmpty() ) {
	    if ( ascending ) {
		return "";
	    } else {
		return TQString::null;
	    }
	} else {
	    return sourceTextKey;	
	}
    } else if ( column == TargetTextShown ) {
	return targetTextKey;
    } else {
	return TQChar( '0' + akey ) + text( column );
    }
}

void PhraseLVI::setText( int column, const TQString& text )
{
    if ( column == SourceTextShown ) {
	sourceTextKey = makeKey( text );
    } else if ( column == TargetTextShown ) {
	targetTextKey = makeKey( text );
    }
    TQListViewItem::setText( column, text );
}

void PhraseLVI::setPhrase( const Phrase& phrase )
{
    setText( SourceTextShown, phrase.source().simplifyWhiteSpace() );
    setText( TargetTextShown, phrase.target().simplifyWhiteSpace() );
    setText( DefinitionText, phrase.definition() );
    setText( SourceTextOriginal, phrase.source() );
    setText( TargetTextOriginal, phrase.target() );
}

Phrase PhraseLVI::phrase() const
{
    return Phrase( text(SourceTextOriginal), text(TargetTextOriginal),
		   text(DefinitionText) );
}

TQString PhraseLVI::makeKey( const TQString& text ) const
{
    if ( text == NewPhrase )
	return TQString::null;

    TQString key;
    for ( int i = 0; i < (int) text.length(); i++ ) {
	if ( text[i] != TQChar('&') )
	    key += text[i].lower();
    }
    // see Section 5, Exercise 4 of The Art of Computer Programming
    key += TQChar::null;
    key += text;
    return key;
}

PhraseLV::PhraseLV( TQWidget *parent, const char *name )
    : TQListView( parent, name )
{
    setAllColumnsShowFocus( true );
    setShowSortIndicator( true );
    for ( int i = 0; i < 3; i++ )
	addColumn( TQString::null, 120 );
    setColumnText( PhraseLVI::SourceTextShown, tr("Source phrase") );
    setColumnText( PhraseLVI::TargetTextShown, tr("Translation") );
    setColumnText( PhraseLVI::DefinitionText, tr("Definition") );
    header()->setStretchEnabled( true, -1 );
    what = new WhatPhrase( this );
}

PhraseLV::~PhraseLV()
{
// delete what;
}

TQSize PhraseLV::sizeHint() const
{
    return TQSize( TQListView::sizeHint().width(), 50 );
}
