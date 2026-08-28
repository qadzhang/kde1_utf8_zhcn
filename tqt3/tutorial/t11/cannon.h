/****************************************************************
**
** Definition of CannonField class, TQt tutorial 11
**
****************************************************************/

#ifndef CANNON_H
#define CANNON_H

class TQTimer;


#include <ntqwidget.h>


class CannonField : public TQWidget
{
    TQ_OBJECT
public:
    CannonField( TQWidget *parent=0, const char *name=0 );

    int   angle() const { return ang; }
    int   force() const { return f; }
    TQSizePolicy sizePolicy() const;

public slots:
    void  setAngle( int degrees );
    void  setForce( int newton );
    void  shoot();

private slots:
    void  moveShot();

signals:
    void  angleChanged( int );
    void  forceChanged( int );

protected:
    void  paintEvent( TQPaintEvent * );

private:
    void  paintShot( TQPainter * );
    void  paintCannon( TQPainter * );
    TQRect cannonRect() const;
    TQRect shotRect() const;

    int ang;
    int f;

    int timerCount;
    TQTimer * autoShootTimer;
    float shoot_ang;
    float shoot_f;
};


#endif // CANNON_H
