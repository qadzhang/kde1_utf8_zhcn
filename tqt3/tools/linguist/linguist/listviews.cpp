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

#include "listviews.h"

#include <ntqpainter.h>

static const int Text0MaxLen = 150;
static const int Text1MaxLen = 150;

/*
   LVI implementation
*/

static TQString rho( int n )
{
    TQString s;
    s.sprintf( "%.10d", n );
    return s;
}

int LVI::count = 0;

LVI::LVI( TQListView * parent, TQString text )
    : TQListViewItem( parent, text )
{
    setText( 0, rho(count++) );
}

LVI::LVI( TQListViewItem * parent, TQString text )
    : TQListViewItem( parent, text )
{
    setText( 0, rho(count++) );
}

/*
    This is a cut-down version of TQListViewItem::paintCell() -
    it is needed to produce the effect we want on obsolete items
 */
void LVI::drawObsoleteText( TQPainter * p, const TQColorGroup & cg, int column,
			    int width, int align )
{
    TQListView * lv = listView();
    int marg = lv ? lv->itemMargin() : 1;
    int r = marg;
    p->fillRect( 0, 0, width, height(), cg.brush( TQColorGroup::Base ) );
    if ( isSelected() && (column==0 || listView()->allColumnsShowFocus()) )
	p->fillRect( r - marg, 0, width - r + marg, height(),
		     cg.brush( TQColorGroup::Highlight ) );
	    
    // Do the ellipsis thingy
    TQString t = text( column );
    TQString tmp;
    int i  = 0;
    if ( p->fontMetrics().width( t ) > width ) {
	tmp = "...";
	while ( p->fontMetrics().width( tmp + t[i] ) < width )
	    tmp += t[ i++ ];
	tmp.remove( (uint)0, 3 );
	if ( tmp.isEmpty() )
	    tmp = t.left( 1 );
	tmp += "...";
	t = tmp;
    }
    if ( isSelected() )
	p->setPen( lv->palette().disabled().highlightedText() );
    else
	p->setPen( lv->palette().disabled().text() );
    
    if ( !t.isEmpty() ) {
	p->drawText( r, 0, width-marg-r, height(),
		     align | AlignVCenter | SingleLine, t );
    }
	    
}

int LVI::compare( TQListViewItem *other, int column, bool ascending ) const
{
    TQString thisKey = key( column, ascending );
    TQString otherKey = other->key( column, ascending );

    if ( thisKey.contains('&') || otherKey.contains('&') ) {
	TQString nicerThisKey = thisKey;
	TQString nicerOtherKey = otherKey;

	nicerThisKey.replace( "&", "" );
	nicerOtherKey.replace( "&", "" );

	int delta = nicerThisKey.localeAwareCompare( nicerOtherKey );
	if ( delta != 0 )
	    return delta;
    }
    return thisKey.localeAwareCompare( otherKey );
}

static TQString fixEllipsis( const TQString & str, int len )
{
    TQString shortened = str.simplifyWhiteSpace();
    if ( (int) shortened.length() > len ) {
	TQString dots = TrWindow::tr( "..." );
	shortened.truncate( len - dots.length() );
	shortened.append( dots );
    }
    return shortened;
}

/*
   MessageLVI implementation
*/
MessageLVI::MessageLVI( TQListView *parent,
			const MetaTranslatorMessage & message,
			const TQString& text, const TQString& comment,
			ContextLVI * c )
    : LVI( parent ), m( message ), tx( text ), com( comment ), ctxt( c )
{
    if ( m.translation().isEmpty() ) {
	TQString t = "";
	m.setTranslation( t );
    }
    setText( 1, fixEllipsis( text, Text0MaxLen ) );
    fini = true;
    d = false;

    if( m.type() ==  MetaTranslatorMessage::Unfinished )
 	setFinished( false );
}

void MessageLVI::updateTranslationText()
{
    setText( 2, fixEllipsis( m.translation(), Text1MaxLen ) );
}

void MessageLVI::paintCell( TQPainter * p, const TQColorGroup & cg, int column, 
			    int width, int align )
{    
    if ( column == 0 ) {
	int x = (width/2) - TrWindow::pxOn->width()/2;
	int y = (height()/2) - TrWindow::pxOn->height()/2;

	int marg = listView() ? listView()->itemMargin() : 1;
	int r = marg;
	
	if ( isSelected() )
	    p->fillRect( r - marg, 0, width - r + marg, height(),
			 cg.brush( TQColorGroup::Highlight ) );
	else
	    p->fillRect( 0, 0, width, height(), 
			 cg.brush( TQColorGroup::Base ) );
		
  	if ( m.type() == MetaTranslatorMessage::Unfinished && danger() )
  	    p->drawPixmap( x, y, *TrWindow::pxDanger );
  	else if ( m.type() == MetaTranslatorMessage::Finished )
  	    p->drawPixmap( x, y, *TrWindow::pxOn );
  	else if ( m.type() == MetaTranslatorMessage::Unfinished )
  	    p->drawPixmap( x, y, *TrWindow::pxOff );
  	else if ( m.type() == MetaTranslatorMessage::Obsolete )
  	    p->drawPixmap( x, y, *TrWindow::pxObsolete );
    } else {
	if ( m.type() == MetaTranslatorMessage::Obsolete )
	    drawObsoleteText( p, cg, column, width, align );
	else
	    TQListViewItem::paintCell( p, cg, column, width, align );
    }
}


void MessageLVI::setTranslation( const TQString& translation )
{
    m.setTranslation( translation );
}

void MessageLVI::setFinished( bool finished )
{
    if ( !fini && finished ) {
	m.setType( MetaTranslatorMessage::Finished );
	repaint();
	ctxt->decrementUnfinishedCount();
    } else if ( fini && !finished ) {
	m.setType( MetaTranslatorMessage::Unfinished );
	repaint();
	ctxt->incrementUnfinishedCount();
    }
    fini = finished;
}

void MessageLVI::setDanger( bool danger )
{
    if ( !d && danger ) {
	ctxt->incrementDangerCount();
	repaint();
    } else if ( d && !danger ) {
	ctxt->decrementDangerCount();
	repaint();
    }
    d = danger;
}

TQString MessageLVI::context() const
{
    return TQString( m.context() );
}

MetaTranslatorMessage MessageLVI::message() const
{
    return m;
}

/*
   ContextLVI implementation
*/
ContextLVI::ContextLVI( TQListView *lv, const TQString& context  )
    : LVI( lv ), com( "" )
{
    messageItems.setAutoDelete( true );

    unfinishedCount = 0;
    dangerCount   = 0;
    obsoleteCount = 0;
    itemCount = 0;
    setText( 1, context );
}

void ContextLVI::instantiateMessageItem( TQListView * lv, MessageLVI * i )
{ 
    itemCount++;
    appendMessageItem( lv, i );
}

void ContextLVI::appendMessageItem( TQListView * lv, MessageLVI * i )
{ 
    lv->takeItem( i );
    messageItems.append( i );
}

void ContextLVI::updateStatus()
{
    TQString s;
    s.sprintf( "%d/%d", itemCount - unfinishedCount - obsoleteCount, 
	       itemCount - obsoleteCount );
    setText( 2, s );
}

void ContextLVI::paintCell( TQPainter * p, const TQColorGroup & cg, int column, 
		    int width, int align )
{
    if ( column == 0 ) {
	int x = (width/2) - TrWindow::pxOn->width()/2;
	int y = (height()/2) - TrWindow::pxOn->height()/2;

	int marg = listView() ? listView()->itemMargin() : 1;
	int r = marg;
	
	if ( isSelected() )
	    p->fillRect( r - marg, 0, width - r + marg, height(),
			 cg.brush( TQColorGroup::Highlight ) );
	else
	    p->fillRect( 0, 0, width, height(),
			 cg.brush( TQColorGroup::Base ) );
		
	if ( isContextObsolete() )
  	    p->drawPixmap( x, y, *TrWindow::pxObsolete );
  	else if ( unfinishedCount == 0 )
  	    p->drawPixmap( x, y, *TrWindow::pxOn );
  	else
  	    p->drawPixmap( x, y, *TrWindow::pxOff );

    } else {
	if ( isContextObsolete() )
	    drawObsoleteText( p, cg, column, width, align );
	else
	    TQListViewItem::paintCell( p, cg, column, width, align );
    }
}

void ContextLVI::appendToComment( const TQString& x )
{
    if ( !com.isEmpty() )
	com += TQString( "\n\n" );
    com += x;
}

void ContextLVI::incrementUnfinishedCount()
{
    if ( unfinishedCount++ == 0 )
	repaint();
}

void ContextLVI::decrementUnfinishedCount()
{
    if ( --unfinishedCount == 0 )
	repaint();
}

void ContextLVI::incrementDangerCount()
{
    if ( dangerCount++ == 0 )
	repaint();
}

void ContextLVI::decrementDangerCount()
{
    if ( --dangerCount == 0 )
	repaint();
}

void ContextLVI::incrementObsoleteCount()
{
    if ( obsoleteCount++ == 0 )
	repaint();
}

bool ContextLVI::isContextObsolete()
{
    return (obsoleteCount == itemCount);
}

TQString ContextLVI::fullContext() const
{
    return comment().stripWhiteSpace();
}
