/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __KAST_TOPLEVEL_H__
#define __KAST_TOPLEVEL_H__

#include <ntqmainwindow.h>
#include <ntqdict.h>
#include <ntqmap.h>

#include "view.h"


class KALedMeter;
class TQLCDNumber;

class KAstTopLevel : public TQMainWindow
{
    TQ_OBJECT
public:
    KAstTopLevel( TQWidget *parent=0, const char *name=0 );
    virtual ~KAstTopLevel();

private:
    void playSound( const char *snd );
    void readSoundMapping();
    void doStats();

protected:
    virtual void showEvent( TQShowEvent * );
    virtual void hideEvent( TQHideEvent * );
    virtual void keyPressEvent( TQKeyEvent *event );
    virtual void keyReleaseEvent( TQKeyEvent *event );

private slots:
    void slotNewGame();

    void slotShipKilled();
    void slotRockHit( int size );
    void slotRocksRemoved();

    void slotUpdateVitals();

private:
    KAsteroidsView *view;
    TQLCDNumber *scoreLCD;
    TQLCDNumber *levelLCD;
    TQLCDNumber *shipsLCD;

    TQLCDNumber *teleportsLCD;
//    TQLCDNumber *bombsLCD;
    TQLCDNumber *brakesLCD;
    TQLCDNumber *shieldLCD;
    TQLCDNumber *shootLCD;
    KALedMeter *powerMeter;

    bool   sound;
    TQDict<TQString> soundDict;

    // waiting for user to press Enter to launch a ship
    bool waitShip;
    bool isPaused;

    int shipsRemain;
    int score;
    int level;
    bool showHiscores;

    enum Action { Launch, Thrust, RotateLeft, RotateRight, Shoot, Teleport,
                    Brake, Shield, Pause, NewGame  };

    TQMap<int,Action> actions;
};

#endif

