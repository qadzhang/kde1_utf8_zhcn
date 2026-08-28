 /**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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
** Licensees holding valid TQt Commercial licenses may use this file in
** accordance with the TQt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#include "timestamp.h"
#include <ntqfile.h>
#include <ntqfileinfo.h>

class TQTimer;

TimeStamp::TimeStamp( TQObject *parent, const TQString &f )
    : TQObject( parent ), filename( f ), autoCheck( false )
{
    timer = new TQTimer( this );
    connect( timer, TQ_SIGNAL( timeout() ), this, TQ_SLOT( autoCheckTimeStamp() ) );
    update();
}

void TimeStamp::setFileName( const TQString &f )
{
    filename = f;
    update();
}

TQString TimeStamp::fileName() const
{
    return filename;
}

void TimeStamp::setAutoCheckEnabled( bool a )
{
    autoCheck = a;
    if ( autoCheck )
	timer->start( 5000 );
    else
	timer->stop();
}

void TimeStamp::update()
{
    TQFile f( filename );
    if ( !f.exists() )
	return;
    lastTimeStamp = TQFileInfo( f ).lastModified();
}

bool TimeStamp::isUpToDate() const
{
    TQFile f( filename );
    if ( !f.exists() )
	return true;
    return lastTimeStamp == TQFileInfo( f ).lastModified();
}

bool TimeStamp::isAutoCheckEnabled() const
{
    return autoCheck;
}

void TimeStamp::autoCheckTimeStamp()
{
    if ( !isUpToDate() )
	emit timeStampChanged();
}
