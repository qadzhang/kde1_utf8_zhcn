/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef FontDisplayer_H
#define FontDisplayer_H

#include <ntqframe.h>
#include <ntqmainwindow.h>

class TQSlider;

class FontRowTable : public TQFrame {
    TQ_OBJECT
public:
    FontRowTable( TQWidget* parent=0, const char* name=0 );

    TQSize sizeHint() const;

signals:
    void fontInformation(const TQString&);

public slots:
    void setRow(int);
    void chooseFont();


protected:
    TQSize cellSize() const;
    void paintEvent( TQPaintEvent* );
private:
    TQFont tablefont;
    int row;
};

class FontDisplayer : public TQMainWindow {
    TQ_OBJECT
public:
    FontDisplayer( TQWidget* parent=0, const char* name=0 );
};

#endif
