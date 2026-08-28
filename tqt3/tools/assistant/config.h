/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the TQt Assistant.
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

#ifndef CONFIG_H
#define CONFIG_H

#include "profile.h"

#include <ntqstring.h>
#include <ntqstringlist.h>
#include <ntqpixmap.h>
#include <ntqmap.h>

class Profile;

class Config
{
public:

    Config();

    void load();
    void save();
    Profile *profile() const { return profil; }
    TQString profileName() const { return profil->props["name"]; }
    bool validProfileName() const;
    void hideSideBar( bool b );
    bool sideBarHidden() const;
    TQStringList mimePaths();

    // From profile, read only
    TQStringList docFiles() const;
    TQStringList docTitles() const;
    TQString indexPage( const TQString &title ) const;
    TQString docImageDir( const TQString &title ) const;
    TQPixmap docIcon( const TQString &title ) const;

    TQStringList profiles() const;
    TQString title() const;
    TQString aboutApplicationMenuText() const;
    TQString aboutURL() const;
    TQPixmap applicationIcon() const;

    // From TQSettings, read / write
    TQString webBrowser() const { return webBrows; }
    void setWebBrowser( const TQString &cmd ) { webBrows = cmd; }

    TQString homePage() const;
    void setHomePage( const TQString &hom ) { home = hom; }

    TQString pdfReader() const { return pdfApp; }
    void setPdfReader( const TQString &cmd ) { pdfApp = cmd; }

    int fontSize() const { return fontSiz; }
    void setFontSize( int size ) { fontSiz = size; }

    TQString fontFamily() const { return fontFam; }
    void setFontFamily( const TQString &fam ) { fontFam = fam; }

    TQString fontFixedFamily() const { return fontFix; }
    void setFontFixedFamily( const TQString &fn ) { fontFix = fn; }

    TQString linkColor() const { return linkCol; }
    void setLinkColor( const TQString &col ) { linkCol = col; }

    TQStringList source() const;
    void setSource( const TQStringList &s ) { src = s; }

    int sideBarPage() const { return sideBar; }
    void setSideBarPage( int sbp ) { sideBar = sbp; }

    TQRect geometry() const { return geom; }
    void setGeometry( const TQRect &geo ) { geom = geo; }

    bool isMaximized() const { return maximized; }
    void setMaximized( bool max ) { maximized = max; }

    bool isLinkUnderline() const { return linkUnder; }
    void setLinkUnderline( bool ul ) { linkUnder = ul; }

    TQString mainWindowLayout() const { return mainWinLayout; }
    void setMainWindowLayout( const TQString &layout ) { mainWinLayout = layout; }

    TQString assistantDocPath() const;

    bool docRebuild() const { return rebuildDocs; }
    void setDocRebuild( bool rb ) { rebuildDocs = rb; }

    void saveProfile( Profile *profile );
    void loadDefaultProfile();

    static Config *configuration();
    static Config *loadConfig(const TQString &profileFileName);

private:
    Config( const Config &c );
    Config& operator=( const Config &c );

    void saveSettings();

private:
    Profile *profil;

    TQStringList profileNames;
    TQString webBrows;
    TQString home;
    TQString pdfApp;
    TQString fontFam;
    TQString fontFix;
    TQString linkCol;
    TQStringList src;
    TQString mainWinLayout;
    TQRect geom;
    int sideBar;
    int fontSiz;
    bool maximized;
    bool linkUnder;
    bool hideSidebar;
    bool rebuildDocs;
};

#endif
