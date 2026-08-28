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

#ifndef LISTVIEWS_H
#define LISTVIEWS_H

#include "msgedit.h"

#include <ntqstring.h>
#include <ntqptrlist.h>
#include <ntqregexp.h>
#include <ntqlistview.h>

class LVI : public TQListViewItem
{
public:
    LVI( TQListView *parent, TQString text = TQString::null );
    LVI( TQListViewItem *parent, TQString text = TQString::null );
    virtual int compare( TQListViewItem *other, int column,
			 bool ascending ) const;
    virtual bool danger() const { return false; }

protected:
    void drawObsoleteText( TQPainter * p, const TQColorGroup & cg, int column,
			   int width, int align );

private:
    static int count;
};

class MessageLVI;
class ContextLVI : public LVI
{
public:
    ContextLVI( TQListView *lv, const TQString& context );

    virtual bool danger() const { return dangerCount > 0; }

    void appendToComment( const TQString& x );
    void incrementUnfinishedCount();
    void decrementUnfinishedCount();
    void incrementDangerCount();
    void decrementDangerCount();
    void incrementObsoleteCount();
    bool isContextObsolete();
    void updateStatus();

    TQString context() const { return text( 1 ); }
    TQString comment() const { return com; }
    TQString fullContext() const;
    bool    finished() const { return unfinishedCount == 0; }

    MessageLVI * firstMessageItem() { return messageItems.first(); }
    MessageLVI * nextMessageItem() { return messageItems.next(); }
    MessageLVI * takeMessageItem( int i ) { return messageItems.take( i ); }
    void         appendMessageItem( TQListView * lv, MessageLVI * i );
    void         instantiateMessageItem( TQListView * lv, MessageLVI * i );
    int          messageItemsInList() { return messageItems.count(); }

    void paintCell( TQPainter * p, const TQColorGroup & cg, int column,
		    int width, int align );
private:
    TQPtrList<MessageLVI> messageItems;
    TQString com;
    int unfinishedCount;
    int dangerCount;
    int obsoleteCount;
    int itemCount;
};

class MessageLVI : public LVI
{
public:
    MessageLVI( TQListView *parent, const MetaTranslatorMessage & message,
		const TQString& text, const TQString& comment, ContextLVI * c );

    virtual bool danger() const { return d; }

    void setTranslation( const TQString& translation );
    void setFinished( bool finished );
    void setDanger( bool danger );

    void setContextLVI( ContextLVI * c ) { ctxt = c; }
    ContextLVI * contextLVI() const { return ctxt; }
    void updateTranslationText();
    
    TQString context() const;
    TQString sourceText() const { return tx; }
    TQString comment() const { return com; }
    TQString translation() const { return m.translation(); }
    bool finished() const { return fini; }
    MetaTranslatorMessage message() const;

    void paintCell( TQPainter * p, const TQColorGroup & cg, int column,
		    int width, int align );
private:
    MetaTranslatorMessage m;
    TQString tx;
    TQString com;
    bool fini;
    bool d;
    ContextLVI * ctxt;
};

#endif
