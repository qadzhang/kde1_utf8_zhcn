/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <ntqiconview.h>
#include <ntqapplication.h>
#include <ntqdragobject.h>
#include <ntqpixmap.h>
#include <ntqiconset.h>

#include <ntqmime.h>
#include <stdio.h>

class ListenDND : public TQObject
{
    TQ_OBJECT

public:
    ListenDND( TQWidget *w )
        : view( w )
    {}

public slots:
    void dropped( TQDropEvent *mime ) {
        tqDebug( "Dropped Mimesource %p into the view %p", mime, view );
        tqDebug( "  Formats:" );
        int i = 0;
        const char *str = mime->format( i );
        tqDebug( "    %s", str );
        while ( str ) {
            tqDebug( "    %s", str );
            str = mime->format( ++i );
        }
    };
    void moved() {
        tqDebug( "All selected items were moved to another widget" );
    }

protected:
    TQWidget *view;

};

int main( int argc, char **argv )
{
    TQApplication a( argc, argv );

    TQIconView qiconview;
    qiconview.setSelectionMode( TQIconView::Extended );

    for ( unsigned int i = 0; i < 3000; i++ ) {
	TQIconViewItem *item = new TQIconViewItem( &qiconview, TQString( "Item %1" ).arg( i + 1 ) );
	item->setRenameEnabled( true );
    }

    qiconview.setCaption( "TQt Example - Iconview" );

    ListenDND listen_dnd( &qiconview );
    TQObject::connect( &qiconview, TQ_SIGNAL( dropped( TQDropEvent *, const TQValueList<TQIconDragItem> & ) ),
		      &listen_dnd, TQ_SLOT( dropped( TQDropEvent * ) ) );
    TQObject::connect( &qiconview, TQ_SIGNAL( moved() ), &listen_dnd, TQ_SLOT( moved() ) );

    a.setMainWidget( &qiconview );
    qiconview.show();
    qiconview.resize( qiconview.sizeHint() );

    return a.exec();
}

#include "main.moc"
