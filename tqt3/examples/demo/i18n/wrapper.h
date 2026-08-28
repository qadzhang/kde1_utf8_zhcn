#ifndef WRAPPER_H
#define WRAPPER_H

#include <ntqvbox.h>
#include <ntqtranslator.h>


class Wrapper : public TQVBox
{
public:
    Wrapper(TQWidget *parent, int i, const char *name = 0)
	: TQVBox(parent, name, WDestructiveClose), translator(this), id(i)
    {
    }

    TQTranslator translator;
    int id;
};


#endif // WRAPPER_H
