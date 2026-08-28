/****************************************************************
**
** Definition of LCDRange class, TQt tutorial 7
**
****************************************************************/

#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <ntqvbox.h>

class TQSlider;


class LCDRange : public TQVBox
{
    TQ_OBJECT
public:
    LCDRange( TQWidget *parent=0, const char *name=0 );

    int value() const;

public slots:
    void setValue( int );

signals:
    void valueChanged( int );

private:
    TQSlider *slider;
};


#endif // LCDRANGE_H
