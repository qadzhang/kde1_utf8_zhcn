/****************************************************************************
** $Id: ntqinputcontextfactory.h,v 1.1.1.1 2004/05/11 11:16:49 daisuke Exp $
**
** Definition of TQInputContextFactory class
**
** Copyright (C) 2000-2002 Trolltech AS.  All rights reserved.
**
** This file is part of the widgets module of the TQt GUI Toolkit.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.TQPL included in the packaging of this file.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** Licensees holding valid TQt Enterprise Edition or TQt Professional Edition
** licenses may use this file in accordance with the TQt Commercial License
** Agreement provided with the Software.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/pricing.html or email sales@trolltech.com for
**   information about TQt Commercial License Agreements.
** See http://www.trolltech.com/qpl/ for TQPL licensing information.
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#ifndef TQINPUTCONTEXTFACTORY_H
#define TQINPUTCONTEXTFACTORY_H

#ifndef QT_H
#include "ntqstringlist.h"
#endif // QT_H

#ifndef TQT_NO_IM

class TQInputContext;
class TQWidget;

class TQ_EXPORT TQInputContextFactory
{
public:
    static TQStringList keys();
    static TQInputContext *create( const TQString &key, TQWidget *widget ); // should be a toplevel widget
    static TQStringList languages( const TQString &key );
    static TQString displayName( const TQString &key );
    static TQString description( const TQString &key );
};
#endif //TQT_NO_IM

#endif //TQINPUTCONTEXTFACTORY_H
