/****************************************************************
**
** Definition of PrintPanel class, translation tutorial 3
**
****************************************************************/

#ifndef PRINTPANEL_H
#define PRINTPANEL_H

#include <ntqvbox.h>

class PrintPanel : public TQVBox
{
    TQ_OBJECT
public:
    PrintPanel( TQWidget *parent = 0, const char *name = 0 );
};

#endif
