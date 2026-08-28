/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef DISPLAY_H
#define DISPLAY_H

#ifndef QT_H
#include <ntqwidget.h>
#include <ntqframe.h>
#endif // QT_H

class TQTimer;
class TQDial;
class TQLCDNumber;
class TQProgressBar;
class TQSpinBox;
class Screen;
class Curve;

class DisplayWidget : public TQWidget {
    TQ_OBJECT
public:
    DisplayWidget( TQWidget *parent=0, const char *name=0 );

    void run();
    void stop();

protected:
    virtual void showEvent( TQShowEvent * );
    virtual void hideEvent( TQHideEvent * );

private slots:
    void tick();

private:
    Screen *screen;
    TQDial *dial;
    Curve *curve;
    TQSpinBox *spin;
    TQLCDNumber *lcd;
    int lcdval;
    TQProgressBar *bar;
    int tbar;
    TQTimer *timer;
};

class Screen : public TQFrame {
    TQ_OBJECT
public:
    enum { FrameWidth = 3 };
    Screen( TQWidget *parent=0, const char *name=0 );
    ~Screen();
    
    void animate();

public slots:
    void setStep( int s );

protected:
    virtual void drawContents( TQPainter * );
    virtual void resizeEvent( TQResizeEvent * );

private:
    int *yval;
    int pos0; // buffer pointer for x == 0
    int t0;   // time parameter at x == 0
    int step;
};

class Curve : public TQFrame {
    TQ_OBJECT
    enum { FrameWidth = 3 };
public:
    Curve( TQWidget *parent=0, const char *name=0 );

    void animate();
public slots:
    void setFactor( int );

protected:
    virtual void drawContents( TQPainter * );

private:
    int shift, n;
};

#endif // PLOT_H
