/****************************************************************
**
** Definition of GameBoard class, TQt tutorial 14
**
****************************************************************/

#ifndef GAMEBRD_H
#define GAMEBRD_H

#include <ntqwidget.h>

class TQPushButton;
class LCDRange;
class TQLCDNumber;
class CannonField;

#include "lcdrange.h"
#include "cannon.h"


class GameBoard : public TQWidget
{
    TQ_OBJECT
public:
    GameBoard( TQWidget *parent=0, const char *name=0 );

protected slots:
    void  fire();
    void  hit();
    void  missed();
    void  newGame();

private:
    TQLCDNumber  *hits;
    TQLCDNumber  *shotsLeft;
    CannonField *cannonField;
};


#endif // GAMEBRD_H
