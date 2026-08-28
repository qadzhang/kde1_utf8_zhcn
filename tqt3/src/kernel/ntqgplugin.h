/****************************************************************************
**
** ...
**
** Copyright (C) 2001-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the TQt GUI Toolkit.
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

#ifndef TQGPLUGIN_H
#define TQGPLUGIN_H

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

#ifndef QT_H
#include "ntqobject.h"
#endif // QT_H

#ifndef TQT_NO_COMPONENT

#ifndef TQ_EXTERN_C
#ifdef __cplusplus
#define TQ_EXTERN_C    extern "C"
#else
#define TQ_EXTERN_C    extern
#endif
#endif

#ifndef TQ_EXPORT_PLUGIN
#if defined(TQT_THREAD_SUPPORT)
#define QT_THREADED_BUILD 1
#define Q_PLUGIN_FLAGS_STRING "11"
#else
#define QT_THREADED_BUILD 0
#define Q_PLUGIN_FLAGS_STRING "01"
#endif

// this is duplicated at Q_UCM_VERIFICATION_DATA in qcom_p.h
// NOTE: if you change pattern, you MUST change the pattern in
// qcomlibrary.cpp as well.  changing the pattern will break all
// backwards compatibility as well (no old plugins will be loaded).
#ifndef Q_PLUGIN_VERIFICATION_DATA
#  define Q_PLUGIN_VERIFICATION_DATA \
	static const char *qt_ucm_verification_data =			\
            "pattern=" "QT_UCM_VERIFICATION_DATA" "\n"			\
            "version=" TQT_VERSION_STR "\n"				\
            "flags=" Q_PLUGIN_FLAGS_STRING "\n"				\
	    "buildkey=" TQT_BUILD_KEY "\0";
#endif // Q_PLUGIN_VERIFICATION_DATA

#define Q_PLUGIN_INSTANTIATE( IMPLEMENTATION )	\
	{ \
	    IMPLEMENTATION *i = new IMPLEMENTATION;	\
	    return i->iface(); \
	}

#define Q_STDCALL

#define TQ_EXPORT_PLUGIN(PLUGIN) \
	    Q_PLUGIN_VERIFICATION_DATA \
	    TQ_EXTERN_C TQ_EXPORT \
            const char * Q_STDCALL qt_ucm_query_verification_data() \
            { return qt_ucm_verification_data; } \
	    TQ_EXTERN_C TQ_EXPORT TQUnknownInterface* Q_STDCALL ucm_instantiate() \
            Q_PLUGIN_INSTANTIATE( PLUGIN )
#endif

struct TQUnknownInterface;

class TQ_EXPORT TQGPlugin : public TQObject
{
    TQ_OBJECT
public:
    TQGPlugin( TQUnknownInterface *i );
    ~TQGPlugin();

    TQUnknownInterface* iface();
    void setIface( TQUnknownInterface *iface );

private:
    TQGPlugin();
    TQUnknownInterface* _iface;
};

#endif // TQT_NO_COMPONENT

#endif // TQGPLUGIN_H
