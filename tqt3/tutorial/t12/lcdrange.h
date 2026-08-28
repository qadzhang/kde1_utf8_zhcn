/****************************************************************
**
** Definition of LCDRange class, TQt tutorial 12
**
****************************************************************/

#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <ntqvbox.h>

class TQSlider;
class TQLabel;


class LCDRange : public TQVBox
{
    TQ_OBJECT
public:
    LCDRange( TQWidget *parent=0, const char *name=0 );
    LCDRange( const char *s, TQWidget *parent=0,
	      const char *name=0 );

    int         value() const;
    const char *text()  const;

public slots:
    void setValue( int );
    void setRange( int minVal, int maxVal );
    void setText( const char * );

signals:
    void valueChanged( int );

private:
    void init();

    TQSlider     *slider;
    TQLabel      *label;
};


#endif // LCDRANGE_H
