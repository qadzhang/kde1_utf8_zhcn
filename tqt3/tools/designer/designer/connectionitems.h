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

#ifndef CONNECTIONITEMS_H
#define CONNECTIONITEMS_H

class FormWindow;

#include <ntqtable.h>

class SenderItem;
class ReceiverItem;
class SignalItem;
class SlotItem;
class ConnectionContainer;


class ConnectionItem : public TQObject,
		       public TQComboTableItem
{
    TQ_OBJECT

public:
    ConnectionItem( TQTable *table, FormWindow *fw );

    void paint( TQPainter *p, const TQColorGroup &cg,
		const TQRect &cr, bool selected );

    void setSender( SenderItem *i );
    void setReceiver( ReceiverItem *i );
    void setSignal( SignalItem *i );
    void setSlot( SlotItem *i );
    void setConnection( ConnectionContainer *c );

public slots:
    virtual void senderChanged( TQObject *sender );
    virtual void receiverChanged( TQObject *receiver );
    virtual void signalChanged( const TQString &sig );
    virtual void slotChanged( const TQString &slot );

signals:
    void changed();

protected:
    FormWindow *formWindow;

private:
    ConnectionContainer *conn;

};

// ------------------------------------------------------------------

class SenderItem : public ConnectionItem
{
    TQ_OBJECT

public:
    SenderItem( TQTable *table, FormWindow *fw );
    TQWidget *createEditor() const;
    void setSenderEx( TQObject *sender );

signals:
    void currentSenderChanged( TQObject *sender );

private slots:
    void senderChanged( const TQString &sender );

};

// ------------------------------------------------------------------

class ReceiverItem : public ConnectionItem
{
    TQ_OBJECT

public:
    ReceiverItem( TQTable *table, FormWindow *fw );
    TQWidget *createEditor() const;
    void setReceiverEx( TQObject *receiver );

signals:
    void currentReceiverChanged( TQObject *receiver );

private slots:
    void receiverChanged( const TQString &receiver );

};

// ------------------------------------------------------------------

class SignalItem : public ConnectionItem
{
    TQ_OBJECT

public:
    SignalItem( TQTable *table, FormWindow *fw );

    void senderChanged( TQObject *sender );
    TQWidget *createEditor() const;

signals:
    void currentSignalChanged( const TQString & );

};

// ------------------------------------------------------------------

class SlotItem : public ConnectionItem
{
    TQ_OBJECT

public:
    SlotItem( TQTable *table, FormWindow *fw );

    void receiverChanged( TQObject *receiver );
    void signalChanged( const TQString &signal );
    TQWidget *createEditor() const;

    void customSlotsChanged();

signals:
    void currentSlotChanged( const TQString & );

private:
    void updateSlotList();
    bool ignoreSlot( const char* slot ) const;

private:
    TQObject *lastReceiver;
    TQString lastSignal;

};

// ------------------------------------------------------------------

class ConnectionContainer : public TQObject
{
    TQ_OBJECT

public:
    ConnectionContainer( TQObject *parent, SenderItem *i1, SignalItem *i2,
			 ReceiverItem *i3, SlotItem *i4, int r )
	: TQObject( parent ), mod( false ), se( i1 ), si( i2 ),
	  re( i3 ), sl( i4 ), rw ( r ) {
	      i1->setConnection( this );
	      i2->setConnection( this );
	      i3->setConnection( this );
	      i4->setConnection( this );
	      connect( i1, TQ_SIGNAL( changed() ), this, TQ_SLOT( somethingChanged() ) );
	      connect( i2, TQ_SIGNAL( changed() ), this, TQ_SLOT( somethingChanged() ) );
	      connect( i3, TQ_SIGNAL( changed() ), this, TQ_SLOT( somethingChanged() ) );
	      connect( i4, TQ_SIGNAL( changed() ), this, TQ_SLOT( somethingChanged() ) );
    }

    bool isModified() const { return mod; }
    void setModified( bool b ) { mod = b; repaint(); }
    bool isValid() const {
	return se->currentText()[0] != '<' &&
		si->currentText()[0] != '<' &&
		re->currentText()[0] != '<' &&
		sl->currentText()[0] != '<';
    }

    void repaint() {
	se->table()->updateCell( se->row(), se->col() );
	si->table()->updateCell( si->row(), si->col() );
	re->table()->updateCell( re->row(), re->col() );
	sl->table()->updateCell( sl->row(), sl->col() );
    }

    int row() const { return rw; }
    void setRow( int r ) { rw = r; }

    SenderItem *senderItem() const { return se; }
    SignalItem *signalItem() const { return si; }
    ReceiverItem *receiverItem() const { return re; }
    SlotItem *slotItem() const { return sl; }

public slots:
    void somethingChanged() { mod = true; emit changed( this ); }

signals:
    void changed( ConnectionContainer *c );

private:
    bool mod;
    SenderItem *se;
    SignalItem *si;
    ReceiverItem *re;
    SlotItem *sl;
    int rw;

};


#endif
