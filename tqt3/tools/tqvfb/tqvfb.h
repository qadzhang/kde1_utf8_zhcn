/**********************************************************************
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt/Embedded virtual framebuffer.
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

#include <ntqmainwindow.h>

class TQVFbView;
class TQVFbRateDialog;
class TQPopupMenu;
class TQMenuData;
class TQFileDialog;
class Config;

class TQVFb: public TQMainWindow
{
    TQ_OBJECT
public:
    TQVFb( int display_id, int w, int h, int d, const TQString &skin, TQWidget *parent = 0,
		const char *name = 0, uint wflags = 0 );
    ~TQVFb();

    void enableCursor( bool e );
    void popupMenu();

protected slots:
    void saveImage();
    void toggleAnimation();
    void toggleCursor();
    void changeRate();
    void about();
    void aboutTQt();

    void configure();

    void setZoom(double);
    void setZoom1();
    void setZoom2();
    void setZoom3();
    void setZoom4();
    void setZoomHalf();

protected:
    void createMenu(TQMenuData *menu);

private:
    void init( int display_id, int w, int h, int d, const TQString &skin );
    TQVFbView *view;
    TQVFbRateDialog *rateDlg;
    TQFileDialog* imagesave;
    TQPopupMenu *viewMenu;
    int cursorId;
    Config* config;
    TQString currentSkin;

private slots:
    void setGamma400(int n);
    void setR400(int n);
    void setG400(int n);
    void setB400(int n);
    void updateGammaLabels();
};

