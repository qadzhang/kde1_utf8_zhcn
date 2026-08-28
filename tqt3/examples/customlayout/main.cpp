/****************************************************************************
**
** Main for custom layout example
**
** Copyright (C) 1996-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "flow.h"
#include "border.h"
#include "card.h"

#include <ntqapplication.h>
#include <ntqlabel.h>
#include <ntqcolor.h>
#include <ntqgroupbox.h>
#include <ntqpushbutton.h>
#include <ntqmultilineedit.h>
#include <ntqcolor.h>

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    TQWidget *f = new TQWidget;
    TQBoxLayout *gm = new TQVBoxLayout( f, 5 );

    SimpleFlow *b1 = new SimpleFlow( gm );

    b1->add( new TQPushButton( "Short", f ) );
    b1->add( new TQPushButton( "Longer", f ) );
    b1->add( new TQPushButton( "Different text", f ) );
    b1->add( new TQPushButton( "More text", f ) );
    b1->add( new TQPushButton( "Even longer button text", f ) );
    TQPushButton* qb = new TQPushButton( "Quit", f );
    a.connect( qb, TQ_SIGNAL( clicked() ), TQ_SLOT( quit() ) );
    b1->add( qb );

    TQWidget *wid = new TQWidget( f );

    BorderLayout *large = new BorderLayout( wid );
    large->setSpacing( 5 );
    large->addWidget( new TQPushButton( "North", wid ), BorderLayout::North );
    large->addWidget( new TQPushButton( "West", wid ), BorderLayout::West );
    TQMultiLineEdit* m = new TQMultiLineEdit( wid );
    m->setText( "Central\nWidget" );
    large->addWidget( m, BorderLayout::Center );
    TQWidget *east1 = new TQPushButton( "East", wid );
    large->addWidget( east1, BorderLayout::East );
    TQWidget *east2 = new TQPushButton( "East 2", wid );
    large->addWidget( east2 , BorderLayout::East );
    large->addWidget( new TQPushButton( "South", wid ), BorderLayout::South );
    //Left-to-right tab order looks better:
    TQWidget::setTabOrder( east2, east1 );
    gm->addWidget( wid );


    wid = new TQWidget( f );
    CardLayout *card = new CardLayout( wid, 10 );

    TQWidget *crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::red );
    card->add( crd );
    crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::green );
    card->add( crd );
    crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::blue );
    card->add( crd );
    crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::white );
    card->add( crd );
    crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::black );
    card->add( crd );
    crd = new TQWidget( wid );
    crd->setBackgroundColor( TQt::yellow );
    card->add( crd );

    gm->addWidget( wid );

    TQLabel* s = new TQLabel( f );
    s->setText( "outermost box" );
    s->setFrameStyle( TQFrame::Panel | TQFrame::Sunken );
    s->setAlignment( TQt::AlignVCenter | TQt::AlignHCenter );
    gm->addWidget( s );
    a.setMainWidget( f );
    f->setCaption("TQt Example - Custom Layout");
    f->show();

    int result = a.exec();
    delete f;
    return result;
}
