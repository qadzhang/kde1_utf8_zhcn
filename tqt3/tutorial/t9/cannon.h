/****************************************************************
**
** Definition of CannonField class, TQt tutorial 8
**
****************************************************************/

#ifndef CANNON_H
#define CANNON_H

#include <ntqwidget.h>


class CannonField : public TQWidget
{
    TQ_OBJECT
public:
    CannonField( TQWidget *parent=0, const char *name=0 );

    int angle() const { return ang; }
    TQSizePolicy sizePolicy() const;

public slots:
    void setAngle( int degrees );

signals:
    void angleChanged( int );

protected:
    void paintEvent( TQPaintEvent * );

private:
    int ang;
};


#endif // CANNON_H
