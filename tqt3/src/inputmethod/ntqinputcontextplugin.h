/****************************************************************************
** $Id: ntqinputcontextplugin.h,v 1.2 2004/06/20 18:43:11 daisuke Exp $
**
** Definition of TQInputContextPlugin class
**
** Created : 010920
**
** Copyright (C) 2001 Trolltech AS.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQINPUTCONTEXTPLUGIN_H
#define TQINPUTCONTEXTPLUGIN_H

#ifndef QT_H
#include "ntqgplugin.h"
#include "ntqstringlist.h"
#endif // QT_H

#ifndef TQT_NO_IM
class TQInputContext;
class TQInputContextPluginPrivate;

class TQ_EXPORT TQInputContextPlugin : public TQGPlugin
{
    TQ_OBJECT
public:
    TQInputContextPlugin();
    ~TQInputContextPlugin();

    virtual TQStringList keys() const = 0;
    virtual TQInputContext *create( const TQString &key ) = 0;
    virtual TQStringList languages( const TQString &key ) = 0;
    virtual TQString displayName( const TQString &key ) = 0;
    virtual TQString description( const TQString &key ) = 0;

private:
    TQInputContextPluginPrivate *d;
};
#endif // TQT_NO_IM
#endif // TQINPUTCONTEXTPLUGIN_H
