/****************************************************************
**
** Definition of ArrowPad class, translation tutorial 2
**
****************************************************************/

#ifndef ARROWPAD_H
#define ARROWPAD_H

#include <ntqgrid.h>

class ArrowPad : public TQGrid
{
    TQ_OBJECT
public:
    ArrowPad( TQWidget *parent = 0, const char *name = 0 );

private:
    void skip();
};

#endif
