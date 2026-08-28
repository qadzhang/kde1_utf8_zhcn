/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "composer.h"
#include "smtp.h"

#include <ntqlineedit.h>
#include <ntqmultilineedit.h>
#include <ntqpushbutton.h>
#include <ntqlabel.h>
#include <ntqlayout.h>

Composer::Composer( TQWidget *parent )
    : TQWidget( parent )
{
    TQGridLayout * layout = new TQGridLayout( this, 1, 1, 6 );

    layout->addWidget( new TQLabel( tr( "From:" ), this ), 0, 0 );
    from = new TQLineEdit( this );
    layout->addWidget( from, 0, 1 );

    layout->addWidget( new TQLabel( tr( "To:" ), this ), 1, 0 );
    to = new TQLineEdit( this );
    layout->addWidget( to, 1, 1 );

    layout->addWidget( new TQLabel( tr( "Subject:" ), this ), 2, 0 );
    subject = new TQLineEdit( this );
    layout->addWidget( subject, 2, 1 );

    message = new TQMultiLineEdit( this );
    layout->addMultiCellWidget( message, 3, 3, 0, 1 );

    send = new TQPushButton( tr( "&Send" ), this );
    layout->addWidget( send, 4, 0 );
    connect( send, TQ_SIGNAL( clicked() ), this, TQ_SLOT( sendMessage() ) );

    sendStatus = new TQLabel( this );
    layout->addWidget( sendStatus, 4, 1 );
}


void Composer::sendMessage()
{
    send->setEnabled( false );
    sendStatus->setText( tr( "Looking up mail servers" ) );
    Smtp *smtp = new Smtp( from->text(), to->text(),
			   subject->text(),
			   message->text() );
    connect( smtp, TQ_SIGNAL(destroyed()),
	     this, TQ_SLOT(enableSend()) );
    connect( smtp, TQ_SIGNAL(status(const TQString &)),
	     sendStatus, TQ_SLOT(setText(const TQString &)) );
}


void Composer::enableSend()
{
    send->setEnabled( true );
}
