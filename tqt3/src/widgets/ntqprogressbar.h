/****************************************************************************
**
** Definition of TQProgressBar class
**
** Created : 970520
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free TQt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** This file may be used under the terms of the Q Public License as
** defined by Trolltech ASA and appearing in the file LICENSE.TQPL
** included in the packaging of this file.  Licensees holding valid TQt
** Commercial licenses may use this file in accordance with the TQt
** Commercial License Agreement provided with the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef TQPROGRESSBAR_H
#define TQPROGRESSBAR_H

#ifndef QT_H
#include "ntqframe.h"
#endif // QT_H

#ifndef TQT_NO_PROGRESSBAR


class TQProgressBarPrivate;


class TQ_EXPORT TQProgressBar : public TQFrame
{
    TQ_OBJECT
    TQ_PROPERTY( int totalSteps READ totalSteps WRITE setTotalSteps )
    TQ_PROPERTY( int progress READ progress WRITE setProgress )
    TQ_PROPERTY( TQString progressString READ progressString )
    TQ_PROPERTY( bool centerIndicator READ centerIndicator WRITE setCenterIndicator )
    TQ_PROPERTY( bool indicatorFollowsStyle READ indicatorFollowsStyle WRITE setIndicatorFollowsStyle )
    TQ_PROPERTY( bool percentageVisible READ percentageVisible WRITE setPercentageVisible )
    TQ_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )

public:
    TQProgressBar( TQWidget* parent=0, const char* name=0, WFlags f=0 );
    TQProgressBar( int totalSteps, TQWidget* parent=0, const char* name=0, WFlags f=0 );
    virtual ~TQProgressBar();

    int		totalSteps() const;
    int		progress()   const;
    const TQString &progressString() const;

    TQSize	sizeHint() const;
    TQSize	minimumSizeHint() const;

    void	setCenterIndicator( bool on );
    bool	centerIndicator() const;

    void        setIndicatorFollowsStyle( bool );
    bool	indicatorFollowsStyle() const;

    bool	percentageVisible() const;
    void	setPercentageVisible( bool );

    void	show();

public slots:
    void	reset();
    virtual void setTotalSteps( int totalSteps );
    virtual void setProgress( int progress );
    void	setProgress( int progress, int totalSteps );

protected:
    void	drawContents( TQPainter * );
    virtual bool setIndicator( TQString & progress_str, int progress,
			       int totalSteps );
    void styleChange( TQStyle& );
    bool	repaintRequired() const;

private:
    int		total_steps;
    int		progress_val;
    int		percentage;
    TQString	progress_str;
    bool        center_indicator    : 1;
    bool        auto_indicator	    : 1;
    bool	percentage_visible  : 1;
    TQProgressBarPrivate * d;
    void         initFrame();

private:	// Disabled copy constructor and operator=
#if defined(TQ_DISABLE_COPY)
    TQProgressBar( const TQProgressBar & );
    TQProgressBar &operator=( const TQProgressBar & );
#endif

public:
    virtual void setOrientation ( Orientation );
    Orientation orientation () const;

private:
    Orientation m_orientation;
};


inline int TQProgressBar::totalSteps() const
{
    return total_steps;
}

inline int TQProgressBar::progress() const
{
    return progress_val;
}

inline const TQString &TQProgressBar::progressString() const
{
    return progress_str;
}

inline bool TQProgressBar::centerIndicator() const
{
    return center_indicator;
}

inline bool TQProgressBar::indicatorFollowsStyle() const
{
    return auto_indicator;
}

inline bool TQProgressBar::percentageVisible() const
{
    return percentage_visible;
}

#endif // TQT_NO_PROGRESSBAR

#endif // TQPROGRESSBAR_H
