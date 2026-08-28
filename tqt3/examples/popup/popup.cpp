/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "popup.h"
#include <ntqapplication.h>
#include <ntqlayout.h>

FancyPopup::FancyPopup( TQWidget* parent, const char*  name ):
    TQLabel( parent, name, WType_Popup ){
        setFrameStyle( WinPanel|Raised );
        setAlignment( AlignCenter );
        resize(150,100);
        moves = 0;
        setMouseTracking( true );
}

void FancyPopup::mouseMoveEvent( TQMouseEvent * e){
    moves++;
    TQString s;
    s.sprintf("%d/%d", e->pos().x(), e->pos().y());
    if (e->state() & TQMouseEvent::LeftButton)
        s += " (down)";
    setText(s);
}

void FancyPopup::mouseReleaseEvent( TQMouseEvent * e){
    if  (rect().contains( e->pos() ) || moves > 5)
        close();
}

void FancyPopup::closeEvent( TQCloseEvent *e ){
    e->accept();
    moves = 0;
    if (!popupParent)
        return;

    // remember that we (as a popup) might recieve the mouse release
    // event instead of the popupParent. This is due to the fact that
    // the popupParent popped us up in its mousePressEvent handler. To
    // avoid the button remaining in pressed state we simply send a
    // faked mouse button release event to it.
    TQMouseEvent me( TQEvent::MouseButtonRelease, TQPoint(0,0), TQPoint(0,0), TQMouseEvent::LeftButton, TQMouseEvent::NoButton);
    TQApplication::sendEvent( popupParent, &me );
}

void FancyPopup::popup( TQWidget* parent) {
    popupParent = parent;
    setText("Move the mouse!");
    if (popupParent)
        move( popupParent->mapToGlobal( popupParent->rect().bottomLeft() ) );
    show();
}






Frame::Frame(TQWidget* parent, const char* name): TQFrame(parent, name){
    button1 = new TQPushButton("Simple Popup", this);
    connect ( button1, TQ_SIGNAL( clicked() ), TQ_SLOT( button1Clicked() ) );
    button2 = new TQPushButton("Fancy Popup", this);
    connect ( button2, TQ_SIGNAL( pressed() ), TQ_SLOT( button2Pressed() ) );

    TQBoxLayout * l = new TQHBoxLayout( this );
    button1->setMaximumSize(button1->sizeHint());
    button2->setMaximumSize(button2->sizeHint());
    l->addWidget( button1 );
    l->addWidget( button2 );
    l->activate();

//     button1->setGeometry(20,20,100,30);
//     button2->setGeometry(140,20,100,30);
    resize(270, 70);

    //create a very simple popup: it is just composed with other
    //widget and will be shown after clicking on button1

    popup1 = new TQFrame( this ,0, WType_Popup);
    popup1->setFrameStyle( WinPanel|Raised );
    popup1->resize(150,100);
    TQLineEdit *tmpE = new TQLineEdit( popup1 );
    connect( tmpE, TQ_SIGNAL( returnPressed() ), popup1, TQ_SLOT( hide() ) );
    tmpE->setGeometry(10,10, 130, 30);
    tmpE->setFocus();
    TQPushButton *tmpB = new TQPushButton("Click me!", popup1);
    connect( tmpB, TQ_SIGNAL( clicked() ), popup1, TQ_SLOT( close() ) );
    tmpB->setGeometry(10, 50, 130, 30);

    // the fancier version uses its own class. It will be shown when
    // pressing button2, so they behavior is more like a modern menu
    // or toolbar.

    popup2 = new FancyPopup( this );

    // you might also add new widgets to the popup, just like you do
    // it with any other widget.  The next four lines (if not
    // commented out) will for instance add a line edit widget.

//     tmpE = new TQLineEdit( popup2 );
//     tmpE->setFocus();
//     connect( tmpE, TQ_SIGNAL( returnPressed() ), popup2, TQ_SLOT( close() ) );
//     tmpE->setGeometry(10, 10, 130, 30);
}


void Frame::button1Clicked(){
    popup1->move( mapToGlobal( button1->geometry().bottomLeft() ) );
    popup1->show();
}

void Frame::button2Pressed(){
    popup2->popup(button2);
}


int main( int argc, char **argv )
{
    TQApplication a(argc,argv);

    Frame frame;
    frame.setCaption("TQt Example - Custom Popups");
    a.setMainWidget(&frame);
    frame.show();
    return a.exec();
}
