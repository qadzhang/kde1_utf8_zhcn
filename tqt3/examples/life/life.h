/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of an example program for TQt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef LIFE_H
#define LIFE_H

#include <ntqframe.h>


class LifeWidget : public TQFrame
{
    TQ_OBJECT
public:
    LifeWidget( int s = 10, TQWidget *parent = 0, const char *name = 0 );

    void	setPoint( int i, int j );

    int		maxCol() { return maxi; }
    int		maxRow() { return maxj; }

public slots:
    void	nextGeneration();
    void	clear();

protected:
    virtual void paintEvent( TQPaintEvent * );
    virtual void mouseMoveEvent( TQMouseEvent * );
    virtual void mousePressEvent( TQMouseEvent * );
    virtual void resizeEvent( TQResizeEvent * );
    void	 mouseHandle( const TQPoint &pos );

private:
    enum { MAXSIZE = 50, MINSIZE = 10, BORDER = 5 };

    bool	cells[2][MAXSIZE + 2][MAXSIZE + 2];
    int		current;
    int		maxi, maxj;

    int pos2index( int x )
    {
	return ( x - BORDER ) / SCALE + 1;
    }
    int index2pos( int i )
    {
	return	( i - 1 ) * SCALE + BORDER;
    }

    int SCALE;
};


#endif // LIFE_H
