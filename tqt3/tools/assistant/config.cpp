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

#include "config.h"
#include "profile.h"
#include "docuparser.h"

#include <ntqapplication.h>
#include <ntqdir.h>
#include <ntqfile.h>
#include <ntqfileinfo.h>
#include <ntqsettings.h>
#include <ntqxml.h>

static Config *static_configuration = 0;

inline TQString getVersionString()
{
    return TQString::number( (TQT_VERSION >> 16) & 0xff )
	+ "." + TQString::number( (TQT_VERSION >> 8) & 0xff );
}

Config::Config()
    : hideSidebar( false ), profil( 0 ), maximized(false)
{
    fontSiz = tqApp->font().pointSize();
    if( !static_configuration ) {
	static_configuration = this;
    } else {
	tqWarning( "Multiple configurations not allowed!" );
    }
}

Config *Config::loadConfig(const TQString &profileFileName)
{
    Config *config = new Config();

    if (profileFileName.isEmpty()) { // no profile
	config->profil = Profile::createDefaultProfile();
	config->load();
	config->loadDefaultProfile();
	return config;
    }

    TQFile file(profileFileName);
    if (!file.exists()) {
	tqWarning( "File does not exist: " + profileFileName );
	return 0;
    }
    DocuParser *parser = DocuParser::createParser( profileFileName );
    if (!parser) {
	tqWarning( "Failed to create parser for file: " + profileFileName );
	return 0;
    }
    if (parser->parserVersion() < DocuParser::TQt320) {
	tqWarning( "File does not contain profile information" );
	return 0;
    }
    DocuParser320 *profileParser = static_cast<DocuParser320*>(parser);
    parser->parse(&file);
    config->profil = profileParser->profile();
    if (!config->profil) {
	tqWarning( "Config::loadConfig(), no profile in: " + profileFileName );
	return 0;
    }
    config->profil->setProfileType(Profile::UserProfile);
    config->profil->setDocuParser(profileParser);
    config->load();
    return config;
}

Config *Config::configuration()
{
    Q_ASSERT( static_configuration );
    return static_configuration;
}

void Config::load()
{
    const TQString key = "/TQt Assistant/" + getVersionString() + "/";
    const TQString profkey = key + "Profile/" + profil->props["name"] + "/";

    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    webBrows = settings.readEntry( key + "Webbrowser" );
    home = settings.readEntry( profkey + "Homepage" );
    pdfApp = settings.readEntry( key + "PDFApplication" );
    linkUnder = settings.readBoolEntry( key + "LinkUnderline", true );
    linkCol = settings.readEntry( key + "LinkColor", "#0000FF" );
    src = settings.readListEntry( profkey + "Source" );
    sideBar = settings.readNumEntry( key + "SideBarPage" );
    if (tqApp->type() != TQApplication::Tty) {
	fontFam = settings.readEntry( key + "Family", tqApp->font().family() );

	fontFix = settings.readEntry( key + "FixedFamily", "courier" );
	fontSiz = settings.readNumEntry( key + "Size", -1 );
	if ( fontSiz < 4 ) {
	    fontSiz = tqApp->font().pointSize();
	}

	geom.setRect( settings.readNumEntry( key + "GeometryX", TQApplication::desktop()->availableGeometry().x() ),
		      settings.readNumEntry( key + "GeometryY", TQApplication::desktop()->availableGeometry().y() ),
		      settings.readNumEntry( key + "GeometryWidth", 800 ),
		      settings.readNumEntry( key + "GeometryHeight", 600 ) );
	maximized = settings.readBoolEntry( key + "GeometryMaximized", false );
    }
    mainWinLayout = settings.readEntry( key + "MainwindowLayout" );
    rebuildDocs = settings.readBoolEntry( key + "RebuildDocDB", true );

    profileNames = settings.entryList( key + "Profile" );
}

void Config::save()
{
    saveSettings();
    saveProfile( profil );
}

void Config::saveSettings()
{
    const TQString key = "/TQt Assistant/" + getVersionString() + "/";
    const TQString profkey = key + "Profile/" + profil->props["name"] + "/";

    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );

    settings.writeEntry( key + "Webbrowser", webBrows );
    settings.writeEntry( profkey + "Homepage", home );
    settings.writeEntry( key + "PDFApplication", pdfApp );
    settings.writeEntry( key + "LinkUnderline", linkUnder );
    settings.writeEntry( key + "LinkColor", linkCol );
    settings.writeEntry( profkey + "Source", src );
    settings.writeEntry( key + "SideBarPage", sideBarPage() );
    if (tqApp->type() != TQApplication::Tty) {
	settings.writeEntry( key + "GeometryX", geom.x() );
	settings.writeEntry( key + "GeometryY", geom.y() );
	settings.writeEntry( key + "GeometryWidth", geom.width() );
	settings.writeEntry( key + "GeometryHeight", geom.height() );
	settings.writeEntry( key + "GeometryMaximized", maximized );
	settings.writeEntry( key + "Family",  fontFam );
	settings.writeEntry( key + "Size",  fontSiz < 4 ? tqApp->font().pointSize() : fontSiz );
	settings.writeEntry( key + "FixedFamily", fontFix );
    }
    if ( !hideSidebar )
	settings.writeEntry( key + "MainwindowLayout", mainWinLayout );
    settings.writeEntry( key + "RebuildDocDB", rebuildDocs );
}

#ifdef ASSISTANT_DEBUG
static void dumpmap( const TQMap<TQString,TQString> &m, const TQString &header )
{
    tqDebug( header );
    TQMap<TQString,TQString>::ConstIterator it = m.begin();
    while (it != m.end()) {
	tqDebug( "  " + it.key() + ":\t\t" + *it );
	++it;
    }
}
#endif

void Config::loadDefaultProfile()
{
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );
    const TQString key = "/TQt Assistant/" + TQString(TQT_VERSION_STR) + "/Profile";
    const TQString profKey = key + "/default/";

    if( settings.entryList( key + "/default" ).count() == 0 ) {
	return;
    }

    // Override the defaults with settings in registry.
    profil->icons.clear();
    profil->indexPages.clear();
    profil->imageDirs.clear();
    profil->docs.clear();
    profil->dcfTitles.clear();

    TQStringList titles = settings.readListEntry( profKey + "Titles" );
    TQStringList iconLst = settings.readListEntry( profKey + "DocIcons" );
    TQStringList indexLst = settings.readListEntry( profKey + "IndexPages" );
    TQStringList imgDirLst = settings.readListEntry( profKey + "ImageDirs" );
    TQStringList dcfs = settings.readListEntry( profKey + "DocFiles" );

    TQStringList::ConstIterator it = titles.begin();
    TQValueListConstIterator<TQString> iconIt = iconLst.begin();
    TQValueListConstIterator<TQString> indexIt = indexLst.begin();
    TQValueListConstIterator<TQString> imageIt = imgDirLst.begin();
    TQValueListConstIterator<TQString> dcfIt = dcfs.begin();
    for( ; it != titles.end();
	++it, ++iconIt, ++indexIt, ++imageIt, ++dcfIt )
    {
	profil->addDCFIcon( *it, *iconIt );
	profil->addDCFIndexPage( *it, *indexIt );
	profil->addDCFImageDir( *it, *imageIt );
	profil->addDCFTitle( *dcfIt, *it );
    }
#if ASSISTANT_DEBUG
    dumpmap( profil->icons, "Icons" );
    dumpmap( profil->indexPages, "IndexPages" );
    dumpmap( profil->imageDirs, "ImageDirs" );
    dumpmap( profil->dcfTitles, "dcfTitles" );
    tqDebug( "Docfiles: \n  " + profil->docs.join( "\n  " ) );
#endif
}

void Config::saveProfile( Profile *profile )
{
    if (profil && profil->profileType() == Profile::UserProfile)
	return;
    TQSettings settings;
    settings.insertSearchPath( TQSettings::Windows, "/Trolltech" );
    TQString versionString = (profile->props["name"] == "default")
	? TQString(TQT_VERSION_STR)
	: getVersionString();
    const TQString key = "/TQt Assistant/" + versionString + "/";
    const TQString profKey = key + "Profile/" + profile->props["name"] + "/";

    TQStringList indexes, icons, imgDirs, dcfs;
    TQValueList<TQString> titles = profile->dcfTitles.keys();
    TQValueListConstIterator<TQString> it = titles.begin();
    for ( ; it != titles.end(); ++it ) {
	indexes << profile->indexPages[*it];
	icons << profile->icons[*it];
	imgDirs << profile->imageDirs[*it];
	dcfs << profile->dcfTitles[*it];
    }

    settings.writeEntry( profKey + "Titles", titles );
    settings.writeEntry( profKey + "DocFiles", dcfs );
    settings.writeEntry( profKey + "IndexPages", indexes );
    settings.writeEntry( profKey + "DocIcons", icons );
    settings.writeEntry( profKey + "ImageDirs", imgDirs );

#if ASSISTANT_DEBUG
    tqDebug( "Titles:\n  - " + ( (TQStringList*) &titles )->join( "\n  - " ) );
    tqDebug( "Docfiles:\n  - " + dcfs.join( "\n  - " ) );
    tqDebug( "IndexPages:\n  - " + indexes.join( "\n  - " ) );
    tqDebug( "DocIcons:\n  - " + icons.join( "\n  - " ) );
    tqDebug( "ImageDirs:\n  - " + imgDirs.join( "\n  - " ) );
#endif
}

TQStringList Config::mimePaths()
{
    static TQStringList lst;

    if( lst.count() > 0 )
	return lst;

    for (TQMap<TQString,TQString>::ConstIterator it = profil->dcfTitles.begin();
	 it != profil->dcfTitles.end(); ++it ) {

	// Mime source for .dcf file path
	TQFileInfo info( *it );
	TQString dcfPath = info.dirPath(true);
	if (lst.contains(dcfPath) == 0)
	    lst << dcfPath;

	// Image dir for .dcf
	TQString imgDir = TQDir::convertSeparators( dcfPath + TQDir::separator()
						  + profil->imageDirs[it.key()] );
	if (lst.contains(imgDir) == 0)
	    lst << imgDir;
    }
    return lst;
}

TQStringList Config::profiles() const
{
    return profileNames;
}

TQString Config::title() const
{
    return profil->props[ "title" ];
}

TQString Config::aboutApplicationMenuText() const
{
    return profil->props[ "aboutmenutext" ];
}

TQString Config::aboutURL() const
{
    return profil->props[ "abouturl" ];
}

TQString Config::homePage() const
{
    return home.isEmpty() ? profil->props["startpage"] : home;
}

TQStringList Config::source() const
{
    return src.size() == 0 ? TQStringList(profil->props["startpage"]) : src;
}

TQStringList Config::docFiles() const
{
    return profil->docs;
}

TQPixmap Config::docIcon( const TQString &title ) const
{
    // ### To allow qdoc generated dcf files to reference the doc icons from qmake_image_col
    if (!TQFile::exists(profil->icons[title]))
	return TQPixmap::fromMimeSource( TQFileInfo(profil->icons[title]).fileName() );
    return TQPixmap::fromMimeSource( profil->icons[title] );
}

TQPixmap Config::applicationIcon() const
{
    return TQPixmap::fromMimeSource( profil->props["applicationicon"] );
}

TQStringList Config::docTitles() const
{
    return TQStringList(profil->indexPages.keys());
}

TQString Config::docImageDir( const TQString &docfile ) const
{
    return profil->imageDirs[docfile];
}

TQString Config::indexPage( const TQString &title ) const
{
    return profil->indexPages
	[title];
}

void Config::hideSideBar( bool b )
{
    hideSidebar = b;
}

bool Config::sideBarHidden() const
{
    return hideSidebar;
}

TQString Config::assistantDocPath() const
{
    return profil->props["assistantdocs"].isEmpty()
	? TQString( tqInstallPathDocs() ) + "/html"
	: profil->props["assistantdocs"];
}
