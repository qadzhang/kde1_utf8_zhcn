/**********************************************************************
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt/Embedded virtual framebuffer.
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

#include "tqvfbratedlg.h"

#include <ntqlayout.h>
#include <ntqlabel.h>
#include <ntqslider.h>
#include <ntqpushbutton.h>

TQVFbRateDialog::TQVFbRateDialog( int rate, TQWidget *parent, const char *name,
    bool modal )
    : TQDialog( parent, name, modal )
{
    oldRate = rate;

    TQVBoxLayout *tl = new TQVBoxLayout( this, 5 );

    TQLabel *label = new TQLabel( "Target frame rate:", this );
    tl->addWidget( label );

    TQHBoxLayout *hl = new TQHBoxLayout( tl );
    rateSlider = new TQSlider( 1, 100, 10, rate, TQSlider::Horizontal, this );
    hl->addWidget( rateSlider );
    connect( rateSlider, TQ_SIGNAL(valueChanged(int)), this, TQ_SLOT(rateChanged(int)) );
    rateLabel = new TQLabel( TQString( "%1fps" ).arg(rate), this );
    hl->addWidget( rateLabel );

    hl = new TQHBoxLayout( tl );
    TQPushButton *pb = new TQPushButton( "OK", this );
    connect( pb, TQ_SIGNAL(clicked()), this, TQ_SLOT(accept()) );
    hl->addWidget( pb );
    pb = new TQPushButton( "Cancel", this );
    connect( pb, TQ_SIGNAL(clicked()), this, TQ_SLOT(cancel()) );
    hl->addWidget( pb );
}

void TQVFbRateDialog::rateChanged( int r )
{
    if ( rateSlider->value() != r )
	rateSlider->setValue( r );
    rateLabel->setText( TQString( "%1fps" ).arg(r) );
    emit updateRate(r);
}

void TQVFbRateDialog::cancel()
{
    rateChanged( oldRate );
    reject();
}
