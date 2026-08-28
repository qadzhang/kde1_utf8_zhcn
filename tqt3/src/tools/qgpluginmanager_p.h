/**********************************************************************
**
** Definition of TQGPluginManager class
**
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the tools module of the TQt GUI Toolkit.
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

#ifndef TQGPLUGINMANAGER_P_H
#define TQGPLUGINMANAGER_P_H

#ifndef QT_H
#include "ntqdict.h"
#include "ntqlibrary.h"
#include "ntquuid.h"
#include "ntqstringlist.h"
#include "qcom_p.h"
#endif // QT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of a number of TQt sources files.  This header file may change from
// version to version without notice, or even be removed.
//
// We mean it.
//
//

#ifndef TQT_NO_COMPONENT

#if defined(Q_TEMPLATEDLL)
// MOC_SKIP_BEGIN
//Q_TEMPLATE_EXTERN template class TQ_EXPORT TQDict<TQLibrary>;
// MOC_SKIP_END
#endif

class TQ_EXPORT TQGPluginManager
{
public:
    TQGPluginManager( const TQUuid& id, const TQStringList& paths = TQString::null, const TQString &suffix = TQString::null, bool cs = true );
    ~TQGPluginManager();

    void addLibraryPath( const TQString& path );
    const TQLibrary* library( const TQString& feature ) const;
    TQStringList featureList() const;

    bool autoUnload() const;
    void setAutoUnload( bool );

protected:
    bool enabled() const;
    bool addLibrary( TQLibrary* plugin );

    TQRESULT queryUnknownInterface(const TQString& feature, TQUnknownInterface** iface) const;

    TQUuid interfaceId;
    TQDict<TQLibrary> plugDict;	    // Dict to match feature with library
    TQDict<TQLibrary> libDict;	    // Dict to match library file with library
    TQStringList libList;

    uint casesens : 1;
    uint autounload : 1;
};

inline void TQGPluginManager::setAutoUnload( bool unload )
{
    autounload = unload;
}

inline bool TQGPluginManager::autoUnload() const
{
    return autounload;
}

#endif

#endif //TQGPLUGINMANAGER_P_H
