/**********************************************************************
**
** Definition of internal TQPSPrinter class.
** TQPSPrinter implements PostScript (tm) output via TQPrinter.
**
** Created : 940927
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
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

#ifndef TQPSPRINTER_P_H
#define TQPSPRINTER_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of qpsprinter.cpp and qprinter_x11.cpp.
// This header file may change from version to version without notice,
// or even be removed.
//
// We mean it.
//
//


#ifndef QT_H
#include "ntqprinter.h"
#include "ntqtextstream.h"
#endif // QT_H

#ifndef TQT_NO_PRINTER

class TQPSPrinterPrivate;

class TQ_EXPORT TQPSPrinter : public TQPaintDevice
{
private:
    // TQPrinter uses these
    TQPSPrinter( TQPrinter *, int );
   ~TQPSPrinter();

    bool cmd ( int, TQPainter *, TQPDevCmdParam * );

    enum { NewPage = 100, AbortPrinting };

    friend class TQPrinter;
private:
    // not used by TQPrinter
    TQPSPrinterPrivate *d;

    // Disabled copy constructor and operator=
    TQPSPrinter( const TQPSPrinter & );
    TQPSPrinter &operator=( const TQPSPrinter & );
};

#endif // TQT_NO_PRINTER

#endif // TQPSPRINTER_P_H
