/****************************************************************************
**
** Definition of something or other
**
** Created : 979899
**
** Copyright (C) 1997-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef POPUP_H
#define POPUP_H
#include <ntqlabel.h>
#include <ntqpushbutton.h>
#include <ntqlineedit.h>

class FancyPopup : public TQLabel
{
    TQ_OBJECT
public:
    FancyPopup( TQWidget* parent = 0, const char*  name=0);
    
    void popup( TQWidget* parent = 0);
protected:
    virtual void mouseMoveEvent( TQMouseEvent * );
    virtual void mouseReleaseEvent( TQMouseEvent * );
    virtual void closeEvent( TQCloseEvent * );
    
private:
    TQWidget* popupParent;
    int moves;
};


 class Frame : public TQFrame
 {
     TQ_OBJECT
 public:
     Frame( TQWidget *parent=0, const char*  name=0);

 protected:

 private slots:
     void button1Clicked();
     void button2Pressed();
     
 private:
     TQPushButton *button1;
     TQPushButton *button2;
     
     TQFrame* popup1;
     FancyPopup* popup2;
 };

#endif
