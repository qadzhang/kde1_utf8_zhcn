/*
 * KAsteroids - Copyright (c) Martin R. Jones 1997
 *
 * Part of the KDE project
 */

#ifndef __LEDMETER_H__
#define __LEDMETER_H__

#include <ntqframe.h>
#include <ntqptrlist.h>


class KALedMeter : public TQFrame
{
    TQ_OBJECT
public:
    KALedMeter( TQWidget *parent );

    int range() const { return mRange; }
    void setRange( int r );

    int count() const { return mCount; }
    void setCount( int c );

    int value () const { return mValue; }

    void addColorRange( int pc, const TQColor &c );

public slots:
    void setValue( int v );

protected:
    virtual void resizeEvent( TQResizeEvent * );
    virtual void drawContents( TQPainter * );
    void calcColorRanges();

protected:
    struct ColorRange
    {
	int mPc;
	int mValue;
	TQColor mColor;
    };

    int mRange;
    int mCount;
    int mCurrentCount;
    int mValue;
    TQPtrList<ColorRange> mCRanges;
};

#endif
