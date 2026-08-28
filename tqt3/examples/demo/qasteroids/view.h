/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __AST_VIEW_H__
#define __AST_VIEW_H__

#include <ntqwidget.h>
#include <ntqptrlist.h>
#include <ntqintdict.h>
#include <ntqtimer.h>
#include <ntqcanvas.h>
#include "sprites.h"

#define MAX_POWER_LEVEL          1000

class KAsteroidsView : public TQWidget
{
    TQ_OBJECT
public:
    KAsteroidsView( TQWidget *parent = 0, const char *name = 0 );
    virtual ~KAsteroidsView();

    int refreshRate;

    void reset();
    void setRockSpeed( double rs ) { rockSpeed = rs; }
    void addRocks( int num );
    void newGame();
    void endGame();
    void newShip();

    void rotateLeft( bool r ) { rotateL = r; rotateSlow = 5; }
    void rotateRight( bool r ) { rotateR = r; rotateSlow = 5; }
    void thrust( bool t ) { thrustShip = t && shipPower > 0; }
    void shoot( bool s ) { shootShip = s; shootDelay = 0; }
    void setShield( bool s );
    void teleport( bool te) { teleportShip = te && mTeleportCount; }
    void brake( bool b );
    void pause( bool p);

    void showText( const TQString &text, const TQColor &color, bool scroll=true );
    void hideText();

    int shots() const { return shotsFired; }
    int hits() const { return shotsHit; }
    int power() const { return shipPower; }

    int teleportCount() const { return mTeleportCount; }
    int brakeCount() const { return mBrakeCount; }
    int shieldCount() const { return mShieldCount; }
    int shootCount() const { return mShootCount; }

signals:
    void shipKilled();
    void rockHit( int size );
    void rocksRemoved();
    void updateVitals();

private slots:
    void hideShield();

protected:
    bool readSprites();
    void wrapSprite( TQCanvasItem * );
    void rockHit( TQCanvasItem * );
    void reducePower( int val );
    void addExhaust( double x, double y, double dx, double dy, int count );
    void processMissiles();
    void processShip();
    void processPowerups();
    void processShield();
    double randDouble();
    int randInt( int range );

    virtual void resizeEvent( TQResizeEvent *event );
    virtual void timerEvent( TQTimerEvent * );

    void showEvent( TQShowEvent * );

private:
    TQCanvas field;
    TQCanvasView view;
    TQIntDict<TQCanvasPixmapArray> animation;
    TQPtrList<TQCanvasSprite> rocks;
    TQPtrList<KMissile> missiles;
    TQPtrList<KBit> bits;
    TQPtrList<KExhaust> exhaust;
    TQPtrList<KPowerup> powerups;
    KShield *shield;
    TQCanvasSprite *ship;
    TQCanvasText *textSprite;

    bool rotateL;
    bool rotateR;
    bool thrustShip;
    bool shootShip;
    bool teleportShip;
    bool brakeShip;
    bool pauseShip;
    bool shieldOn;

    bool vitalsChanged;

    int  shipAngle;
    int  rotateSlow;
    int  rotateRate;
    int  shipPower;

    int shotsFired;
    int shotsHit;
    int shootDelay;

    int mBrakeCount;
    int mShieldCount;
    int mTeleportCount;
    int mShootCount;

    double shipDx;
    double shipDy;

    int  textDy;
    int  mFrameNum;
    bool mPaused;
    int  mTimerId;

    double rockSpeed;
    double powerupSpeed;

    bool can_destroy_powerups;

    TQTimer *shieldTimer;
    bool initialized;
};

#endif
