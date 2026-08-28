/****************************************************************************
**
** Definition of ???
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

#ifndef TQINPUTCONTEXT_P_H
#define TQINPUTCONTEXT_P_H


//
//  W A R N I N G
//  -------------
//
// This file is not part of the TQt API.  It exists for the convenience
// of internal files.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//
//

#include "ntqglobal.h"

class TQKeyEvent;
class TQWidget;
class TQFont;
class TQString;


#ifdef TQ_WS_X11
#include "ntqarray.h"
#include "ntqwindowdefs.h"
#include "qt_x11_p.h"
#endif

#ifdef TQ_WS_WIN
#include "qt_windows.h"
#endif

#ifdef TQ_WS_QWS
class TQWSIMEvent;
#endif

class TQInputContext
{
public:
#ifdef TQ_WS_X11
    TQInputContext(TQWidget *); // should be a toplevel widget
    ~TQInputContext();

    void setFocus();
    void setComposePosition(int, int);
    void setComposeArea(int, int, int, int);
    void reset();

    int lookupString(XKeyEvent *, TQCString &, KeySym *, Status *) const;
    void setXFontSet(const TQFont &);

    void *ic;
    TQString text;
    TQWidget *focusWidget;
    bool composing;
    TQFont font;
    XFontSet fontset;
    TQMemArray<bool> selectedChars;
#endif // TQ_WS_X11

#ifdef TQ_WS_QWS
    static void translateIMEvent( TQWSIMEvent *, TQWidget * );
    static void reset();
private:
    static TQWidget* focusWidget;
    static TQString* composition;
#endif //TQ_WS_QWS

#ifdef TQ_WS_WIN
    static void init();
    static void shutdown();

    static void TranslateMessage( const MSG *msg);
    static LRESULT DefWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

    static void setFont( const TQWidget *w, const TQFont & );
    static void setFocusHint( int x, int y, int w, int h, const TQWidget *widget );
    static bool startComposition();
    static bool endComposition( TQWidget *fw = 0 );
    static bool composition( LPARAM lparam );

    static void accept( TQWidget *fw = 0 );
    static void enable( TQWidget *w, bool b );
#endif
};

#endif // TQINPUTCONTEXT_P_H
