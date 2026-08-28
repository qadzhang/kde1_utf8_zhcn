/****************************************************************
**
** Definition of CannonField class, TQt tutorial 10
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

    TQSizePolicy sizePolicy() const;

    int   angle() const { return ang; }
    int   force() const { return f; }

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );

signals:
    void  angleChanged( int );
    void  forceChanged( int );

protected:
    void  paintEvent( TQPaintEvent * );

private:
    TQRect cannonRect() const;

    int ang;
    int f;
};


#endif // CANNON_H
