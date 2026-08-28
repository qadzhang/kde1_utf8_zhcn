/**********************************************************************
** Copyright (C) 2000-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Configuration.
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

#include "mainwindow.h"
#include "colorbutton.h"
#include "previewframe.h"
#include "paletteeditoradvanced.h"

#include <ntqlabel.h>
#include <ntqapplication.h>
#include <ntqcombobox.h>
#include <ntqstylefactory.h>
#include <ntqinputcontextfactory.h>
#include <ntqobjectlist.h>
#include <ntqfontdatabase.h>
#include <ntqlineedit.h>
#include <ntqlistbox.h>
#include <ntqspinbox.h>
#include <ntqcheckbox.h>
#include <ntqfiledialog.h>
#include <ntqaction.h>
#include <ntqstatusbar.h>
#include <ntqsettings.h>
#include <ntqmessagebox.h>
#include <ntqtextview.h>
#include <ntqstyle.h>

#include <stdlib.h>


// from qapplication.cpp and qapplication_x11.cpp - These are NOT for
// external use ignore them
extern bool TQ_EXPORT tqt_resolve_symlinks;


static const char *appearance_text =
"<p><b><font size+=2>Appearance</font></b></p>"
"<hr>"
"<p>Use this tab to customize the appearance of your TQt applications.</p>"
"<p>You can select the default GUI Style from the drop down list and "
"customize the colors.</p>"
"<p>Any GUI Style plugins in your plugin path will automatically be added "
"to the list of built-in TQt styles. (See the Library Paths tab for "
"information on adding new plugin paths.)</p>"
"<p>When you choose 3-D Effects and Background colors, the TQt Configuration "
"program will automatically generate a palette for you.  To customize "
"colors further, press the Tune Palette button to open the advanced "
"palette editor."
"<p>The Preview Window shows what the selected Style and colors look "
"like.";

static const char *font_text =
"<p><b><font size+=2>Fonts</font></b></p>"
"<hr>"
"<p>Use this tab to select the default font for your TQt applications. "
"The selected font is shown (initially as 'Sample Text') in the line "
"edit below the Family, "
"Style and Point Size drop down lists.</p>"
"<p>TQt has a powerful font substitution feature that allows you to "
"specify a list of substitute fonts.  Substitute fonts are used "
"when a font cannot be loaded, or if the specified font doesn't have "
"a particular character."
"<p>For example, if you select the font Lucida, which doesn't have Korean "
"characters, but need to show some Korean text using the Mincho font family "
"you can do so by adding Mincho to the list. Once Mincho is added, any "
"Korean characters that are not found in the Lucida font will be taken "
"from the Mincho font.  Because the font substitutions are "
"lists, you can also select multiple families, such as Song Ti (for "
"use with Chinese text).";

static const char *interface_text =
"<p><b><font size+=2>Interface</font></b></p>"
"<hr>"
"<p>Use this tab to customize the feel of your TQt applications.</p>"
"<p>If the Resolve Symlinks checkbox is checked TQt will follow symlinks "
"when handling URLs. For example, in the file dialog, if this setting is turned "
"on and /usr/tmp is a symlink to /var/tmp, entering the /usr/tmp directory "
"will cause the file dialog to change to /var/tmp.  With this setting turned "
"off, symlinks are not resolved or followed.</p>"
"<p>The Global Strut setting is useful for people who require a "
"minimum size for all widgets (e.g. when using a touch panel or for users "
"who are visually impaired).  Leaving the Global Strut width and height "
"at 0 will disable the Global Strut feature</p>"
"<p>XIM (Extended Input Methods) are used for entering characters in "
"languages that have large character sets, for example, Chinese and "
"Japanese.";
// ### What does the 'Enhanced support for languages written R2L do?

static const char *libpath_text =
"<p><b><font size+=2>Library Paths</font></b></p>"
"<hr>"
"<p>Use this tab to select additional directories where TQt should search "
"for component plugins."
"<p>These directories should be the base directory of where your plugins "
"are stored.  For example, if you wish to store GUI Style plugins in "
"$HOME/plugins/styles and TQt Designer plugins in $HOME/plugins/designer, "
"you would add $HOME/plugins to your Library Path. <p>Notes:"
"<ul><li>The TQt Configuration program does <i>not</i> support environment "
"variables, so you must give full path names, for example "
"<tt>/home/brad/myplugins</tt> not <tt>$HOME/myplugins</tt>"
"<li> TQt automatically "
"searches in the directory where you installed TQt for component plugins. "
"Removing that path is not possible.</ul>";

static const char *printer_text =
"<p><b><font size+=2>Printer</font></b></p>"
"<hr>"
"<p>Use this tab to configure the way TQt generates output for the printer."
"You can specify if TQt should try to embed fonts into its generated output."
"If you enable font embedding, the resulting postscript will be more "
"portable and will more accurately reflect the "
"visual output on the screen; however the resulting postscript file "
"size will be bigger."
"<p>When using font embedding you can select additional directories where "
"TQt should search for embeddable font files.  By default, the X "
"server font path is used.";

static const char *about_text =
"<p><b><font size=+2>TQt Configuration</font></b></p>"
"<p>A graphical configuration tool for programs using TQt</p>"
"<p>Version 1.0</p>"
"<p>Copyright (C) 2001-2008 Trolltech ASA. All rights reserved.</p>"
"<p></p>"
"<p>This program is licensed to you under the terms of the GNU General "
"Public License Version 2 as published by the Free Software Foundation. This "
"gives you legal permission to copy, distribute and/or modify this software "
"under certain conditions. For details, see the file 'LICENSE.GPL' that came with "
"this software distribution. If you did not get the file, send email to "
"info@trolltech.com.</p>\n\n<p>The program is provided AS IS with NO WARRANTY "
"OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS "
"FOR A PARTICULAR PURPOSE.</p>";


static TQColorGroup::ColorRole centralFromItem( int item )
{
    switch( item ) {
    case 0:  return TQColorGroup::Background;
    case 1:  return TQColorGroup::Foreground;
    case 2:  return TQColorGroup::Button;
    case 3:  return TQColorGroup::Base;
    case 4:  return TQColorGroup::Text;
    case 5:  return TQColorGroup::BrightText;
    case 6:  return TQColorGroup::ButtonText;
    case 7:  return TQColorGroup::Highlight;
    case 8:  return TQColorGroup::HighlightedText;
    default: return TQColorGroup::NColorRoles;
    }
}


static TQColorGroup::ColorRole effectFromItem( int item )
{
    switch( item ) {
    case 0:  return TQColorGroup::Light;
    case 1:  return TQColorGroup::Midlight;
    case 2:  return TQColorGroup::Mid;
    case 3:  return TQColorGroup::Dark;
    case 4:  return TQColorGroup::Shadow;
    default: return TQColorGroup::NColorRoles;
    }
}


static void setStyleHelper(TQWidget *w, TQStyle *s)
{
    w->unsetPalette();
    w->setStyle(s);

    const TQObjectList *children = w->children();
    if (! children)
	return;

    TQPtrListIterator<TQObject> childit(*children);
    TQObject *child;
    while ((child = childit.current()) != 0) {
	++childit;

	if (child->isWidgetType())
	    setStyleHelper((TQWidget *) child, s);
    }
}


MainWindow::MainWindow()
    : MainWindowBase(0, "main window"),
      editPalette(palette()), previewPalette(palette()), previewstyle(0)
{
    modified = true;

    TQStringList gstyles = TQStyleFactory::keys();
    gstyles.sort();
    gstylecombo->insertStringList(gstyles);

    TQSettings settings;
    TQString currentstyle = settings.readEntry("/qt/style");
    if (currentstyle.isNull())
	currentstyle = TQApplication::style().className();
    {
	int s = 0;
	TQStringList::Iterator git = gstyles.begin();
	while (git != gstyles.end()) {
	    if (*git == currentstyle)
		break;
	    s++;
	    git++;
	}

	if (s < gstylecombo->count()) {
	    gstylecombo->setCurrentItem(s);
	} else {
	    // no predefined style, try to find the closest match
	    // class names usually contain the name of the style, so we
	    // iterate over the items in the combobox, and use the one whose
	    // name is contained in the classname of the style
	    s = 0;
	    git = gstyles.begin();
	    while (git != gstyles.end()) {
		if (currentstyle.contains(*git))
		    break;
		s++;
		git++;
	    }

	    if (s < gstylecombo->count()) {
		gstylecombo->setCurrentItem(s);
	    } else {
		// we give up
		gstylecombo->insertItem("Unknown");
		gstylecombo->setCurrentItem(gstylecombo->count() - 1);
	    }
	}
    }

    buttonMainColor->setColor(palette().color(TQPalette::Active,
					      TQColorGroup::Button));
    buttonMainColor2->setColor(palette().color(TQPalette::Active,
					       TQColorGroup::Background));

    TQFontDatabase db;
    TQStringList families = db.families();
    familycombo->insertStringList(families);

    TQStringList fs = families;
    TQStringList fs2 = TQFont::substitutions();
    TQStringList::Iterator fsit = fs2.begin();
    while (fsit != fs2.end()) {
	if (! fs.contains(*fsit))
	    fs += *fsit;
	fsit++;
    }
    fs.sort();
    familysubcombo->insertStringList(fs);

    choosesubcombo->insertStringList(families);
    TQValueList<int> sizes = db.standardSizes();
    TQValueList<int>::Iterator it = sizes.begin();
    while (it != sizes.end())
	psizecombo->insertItem(TQString::number(*it++));

    dcispin->setValue(TQApplication::doubleClickInterval());
    cfispin->setValue(TQApplication::cursorFlashTime());
    wslspin->setValue(TQApplication::wheelScrollLines());
    resolvelinks->setChecked(tqt_resolve_symlinks);

    effectcheckbox->setChecked(TQApplication::isEffectEnabled(UI_General));
    effectbase->setEnabled(effectcheckbox->isChecked());

    if (TQApplication::isEffectEnabled(UI_FadeMenu))
	menueffect->setCurrentItem(2);
    else if (TQApplication::isEffectEnabled(UI_AnimateMenu))
	menueffect->setCurrentItem(1);

    if (TQApplication::isEffectEnabled(UI_AnimateCombo))
	comboeffect->setCurrentItem(1);

    if (TQApplication::isEffectEnabled(UI_FadeTooltip))
	tooltipeffect->setCurrentItem(2);
    else if (TQApplication::isEffectEnabled(UI_AnimateTooltip))
	tooltipeffect->setCurrentItem(1);

    if ( TQApplication::isEffectEnabled( UI_AnimateToolBox ) )
	toolboxeffect->setCurrentItem( 1 );

    TQSize globalStrut = TQApplication::globalStrut();
    strutwidth->setValue(globalStrut.width());
    strutheight->setValue(globalStrut.height());

    libpathlistbox->clear();
    libpathlistbox->insertStringList(TQApplication::libraryPaths());

    // find the default family
    TQStringList::Iterator sit = families.begin();
    int i = 0, possible = -1;
    while (sit != families.end()) {
	if (*sit == TQApplication::font().family())
	    break;
	if ((*sit).contains(TQApplication::font().family()))
	    possible = i;

	i++;
	sit++;
    }
    if (sit == families.end())
	i = possible;
    if (i == -1) // no clue about the current font
	i = 0;

    familycombo->setCurrentItem(i);

    TQStringList styles = db.styles(familycombo->currentText());
    stylecombo->insertStringList(styles);

    TQString stylestring = db.styleString(TQApplication::font());
    sit = styles.begin();
    i = 0;
    possible = -1;
    while (sit != styles.end()) {
	if (*sit == stylestring)
	    break;
	if ((*sit).contains(stylestring))
	    possible = i;

	i++;
	sit++;
    }
    if (sit == styles.end())
	i = possible;
    if (i == -1) // no clue about the current font
	i = 0;
    stylecombo->setCurrentItem(i);

    i = 0;
    while (i < psizecombo->count()) {
	if (psizecombo->text(i) == TQString::number(TQApplication::font().pointSize())) {
	    psizecombo->setCurrentItem(i);
	    break;
	}

	i++;
    }

    TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
    sublistbox->clear();
    sublistbox->insertStringList(subs);

    rtlExtensions->setChecked( settings.readBoolEntry( "/qt/useRtlExtensions", false ) );

#if defined(TQ_WS_X11)
    inputStyle->setCurrentText( trUtf8(
            settings.readEntry( "/qt/XIMInputStyle", TQString::fromLatin1( "On The Spot" ) ) ) );
#else
    inputStyle->hide();
    inputStyleLabel->hide();
#endif

#if defined(TQ_WS_X11) && !defined(TQT_NO_IM_EXTENSIONS)
    /*
        This code makes it possible to set up default input method.

	The list of identifier names of input method which can be used
	is acquired using TQInputContextFactory::keys(). And it is
	translated to display name and set to inputMethodCombo which
	displays the list of input method.
     */
    inputMethodIdentifiers = TQInputContextFactory::keys();
    TQStringList imDispNames;
    {
	// input method switcher should named with "imsw-" prefix to
	// prevent to be listed in ordinary input method list.
	TQStringList::Iterator imIt = inputMethodIdentifiers.begin();
	while (imIt != inputMethodIdentifiers.end()) {
	    if ((*imIt).find("imsw-") == 0)
		imIt = inputMethodIdentifiers.remove(imIt);
	    else
		imIt++;
	}
	// we should not sort the list
	//inputMethodIdentifiers.sort();
	for (imIt = inputMethodIdentifiers.begin();
	     imIt != inputMethodIdentifiers.end();
	     imIt++) {
	    TQString dispName = TQInputContextFactory::displayName(*imIt);
	    if (dispName.isNull() || dispName.isEmpty())
		dispName = *imIt;
	    imDispNames << dispName;
	}
    }
    //inputMethodCombo->insertStringList(inputMethodIdentifiers);
    inputMethodCombo->insertStringList(imDispNames);

    /*
        input method set up as a default in the past is chosen.
	If nothing is set up, default input method in the platform is chosen. 
     */
    // default input method is XIM in X11.
    TQString currentIM = settings.readEntry("/qt/DefaultInputMethod", "xim");
    {
	int index = inputMethodIdentifiers.findIndex(currentIM);
	// set up Selected input method.
	if (0 <= index && index < inputMethodIdentifiers.count()) {
	    inputMethodCombo->setCurrentItem(index);
	} else {
	    // Give up. this part is executed when specified IM is not
	    // installed.
	    TQString dispName = tr("Unknown Input Method") + " (" + currentIM + ")";
	    inputMethodCombo->insertItem(dispName);
	    inputMethodCombo->setCurrentItem(inputMethodCombo->count() - 1);
	    inputMethodIdentifiers << currentIM;
	}
    }
#else
    inputMethodCombo->hide();
    inputMethodLabel->hide();
#endif

    fontembeddingcheckbox->setChecked( settings.readBoolEntry("/qt/embedFonts", true) );
    fontpaths = settings.readListEntry("/qt/fontPath", ':');
    fontpathlistbox->insertStringList(fontpaths);

    setModified(false);
}


MainWindow::~MainWindow()
{
}


void MainWindow::fileSave()
{
    if (! modified) {
	statusBar()->message("No changes to be saved.", 2000);
	return;
    }

    statusBar()->message("Saving changes...");

    {
	TQSettings settings;
	TQFontDatabase db;
	TQFont font = db.font(familycombo->currentText(),
			     stylecombo->currentText(),
			     psizecombo->currentText().toInt());
	TQStringList actcg, inactcg, discg;

	int i;
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    actcg << editPalette.color(TQPalette::Active,
				       (TQColorGroup::ColorRole) i).name();
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    inactcg << editPalette.color(TQPalette::Inactive,
					 (TQColorGroup::ColorRole) i).name();
	for (i = 0; i < TQColorGroup::NColorRoles; i++)
	    discg << editPalette.color(TQPalette::Disabled,
				       (TQColorGroup::ColorRole) i).name();

	settings.writeEntry("/qt/font", font.toString());
	settings.writeEntry("/qt/Palette/active", actcg);
	settings.writeEntry("/qt/Palette/inactive", inactcg);
	settings.writeEntry("/qt/Palette/disabled", discg);

	TQStringList libpath = TQApplication::libraryPaths();
	TQString libpathkey =
	    TQString("/qt/%1.%2/libraryPath").arg( TQT_VERSION >> 16 ).arg( (TQT_VERSION & 0xff00 ) >> 8 );
	settings.writeEntry(libpathkey, libpath, ':');
	settings.writeEntry("/qt/fontPath", fontpaths, ':');
	settings.writeEntry("/qt/embedFonts", fontembeddingcheckbox->isChecked() );
	settings.writeEntry("/qt/style", gstylecombo->currentText());
	settings.writeEntry("/qt/doubleClickInterval",
					     dcispin->value());
	settings.writeEntry("/qt/cursorFlashTime", cfispin->value() == 9 ? 0 : cfispin->value() );
	settings.writeEntry("/qt/wheelScrollLines", wslspin->value());
	settings.writeEntry("/qt/resolveSymlinks", resolvelinks->isChecked());

	TQStringList strut;
	strut << TQString::number(strutwidth->value());
	strut << TQString::number(strutheight->value());
	settings.writeEntry("/qt/globalStrut", strut);

	settings.writeEntry("/qt/useRtlExtensions", rtlExtensions->isChecked() );

#ifdef TQ_WS_X11
	TQString style = inputStyle->currentText();
	TQString str = "On The Spot";
	if ( style == trUtf8( "Over The Spot" ) )
	    str = "Over The Spot";
	else if ( style == trUtf8( "Off The Spot" ) )
	    str = "Off The Spot";
	else if ( style == trUtf8( "Root" ) )
	    str = "Root";
	settings.writeEntry( "/qt/XIMInputStyle", str );

#if !defined(TQT_NO_IM_EXTENSIONS)
	TQString imSwitcher = settings.readEntry("/qt/DefaultInputMethodSwitcher", "imsw-multi");
	settings.writeEntry("/qt/DefaultInputMethodSwitcher", imSwitcher);
	int imIndex = inputMethodCombo->currentItem();
	settings.writeEntry("/qt/DefaultInputMethod", inputMethodIdentifiers[imIndex]);
#endif
#endif

	TQStringList effects;
	if (effectcheckbox->isChecked()) {
	    effects << "general";

	    switch (menueffect->currentItem()) {
	    case 1: effects << "animatemenu"; break;
	    case 2: effects << "fademenu"; break;
	    }

	    switch (comboeffect->currentItem()) {
	    case 1: effects << "animatecombo"; break;
	    }

	    switch (tooltipeffect->currentItem()) {
	    case 1: effects << "animatetooltip"; break;
	    case 2: effects << "fadetooltip"; break;
	    }

	    switch ( toolboxeffect->currentItem() ) {
	    case 1: effects << "animatetoolbox"; break;
	    }
	} else
	    effects << "none";
	settings.writeEntry("/qt/GUIEffects", effects);

	TQStringList familysubs = TQFont::substitutions();
	TQStringList::Iterator fit = familysubs.begin();
	while (fit != familysubs.end()) {
	    TQStringList subs = TQFont::substitutes(*fit);
	    settings.writeEntry("/qt/Font Substitutions/" + *fit, subs);
	    fit++;
	}
    }

#if defined(TQ_WS_X11)
    TQApplication::x11_apply_settings();
#endif // TQ_WS_X11

    setModified(false);
    statusBar()->message("Saved changes.");
}


void MainWindow::fileExit()
{
    tqApp->closeAllWindows();
}


void MainWindow::setModified(bool m)
{
    if (modified == m)
	return;

    modified = m;
    fileSaveAction->setEnabled(m);
}


void MainWindow::buildPalette()
{
    int i;
    TQColorGroup cg;
    TQColor btn = buttonMainColor->color();
    TQColor back = buttonMainColor2->color();
    TQPalette automake( btn, back );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), automake.active().color( centralFromItem(i) ) );

    editPalette.setActive( cg );
    buildActiveEffect();

    cg = editPalette.inactive();

    TQPalette temp( editPalette.active().color( TQColorGroup::Button ),
		   editPalette.active().color( TQColorGroup::Background ) );

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.inactive().color( centralFromItem(i) ) );

    editPalette.setInactive( cg );
    buildInactiveEffect();

    cg = editPalette.disabled();

    for (i = 0; i<9; i++)
	cg.setColor( centralFromItem(i), temp.disabled().color( centralFromItem(i) ) );

    editPalette.setDisabled( cg );
    buildDisabledEffect();

    updateColorButtons();

    setModified(true);
}


void MainWindow::buildActiveEffect()
{
    TQColorGroup cg = editPalette.active();
    TQColor btn = cg.color( TQColorGroup::Button );

    TQPalette temp( btn, btn );

    for (int i = 0; i<5; i++)
	cg.setColor( effectFromItem(i), temp.active().color( effectFromItem(i) ) );

    editPalette.setActive( cg );
    setPreviewPalette( editPalette );

    updateColorButtons();
}


void MainWindow::buildInactive()
{
    editPalette.setInactive( editPalette.active() );
    buildInactiveEffect();
}


void MainWindow::buildInactiveEffect()
{
    TQColorGroup cg = editPalette.inactive();

    TQColor light, midlight, mid, dark, shadow;
    TQColor btn = cg.color( TQColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( TQColorGroup::Light, light );
    cg.setColor( TQColorGroup::Midlight, midlight );
    cg.setColor( TQColorGroup::Mid, mid );
    cg.setColor( TQColorGroup::Dark, dark );
    cg.setColor( TQColorGroup::Shadow, shadow );

    editPalette.setInactive( cg );
    setPreviewPalette( editPalette );
    updateColorButtons();
}


void MainWindow::buildDisabled()
{
    TQColorGroup cg = editPalette.active();
    cg.setColor( TQColorGroup::ButtonText, darkGray );
    cg.setColor( TQColorGroup::Foreground, darkGray );
    cg.setColor( TQColorGroup::Text, darkGray );
    cg.setColor( TQColorGroup::HighlightedText, darkGray );
    editPalette.setDisabled( cg );

    buildDisabledEffect();
}


void MainWindow::buildDisabledEffect()
{
    TQColorGroup cg = editPalette.disabled();

    TQColor light, midlight, mid, dark, shadow;
    TQColor btn = cg.color( TQColorGroup::Button );

    light = btn.light(150);
    midlight = btn.light(115);
    mid = btn.dark(150);
    dark = btn.dark();
    shadow = black;

    cg.setColor( TQColorGroup::Light, light );
    cg.setColor( TQColorGroup::Midlight, midlight );
    cg.setColor( TQColorGroup::Mid, mid );
    cg.setColor( TQColorGroup::Dark, dark );
    cg.setColor( TQColorGroup::Shadow, shadow );

    editPalette.setDisabled( cg );
    setPreviewPalette( editPalette );
    updateColorButtons();
}


void MainWindow::setPreviewPalette( const TQPalette& pal )
{
    TQColorGroup cg;

    switch (paletteCombo->currentItem()) {
    case 0:
    default:
	cg = pal.active();
	break;
    case 1:
	cg = pal.inactive();
	break;
    case 2:
	cg = pal.disabled();
	break;
    }
    previewPalette.setActive( cg );
    previewPalette.setInactive( cg );
    previewPalette.setDisabled( cg );

    previewFrame->setPreviewPalette(previewPalette);
}


void MainWindow::updateColorButtons()
{
    buttonMainColor->setColor( editPalette.active().color( TQColorGroup::Button ));
    buttonMainColor2->setColor( editPalette.active().color( TQColorGroup::Background ));
}


void MainWindow::tunePalette()
{
    bool ok;
    TQPalette pal = PaletteEditorAdvanced::getPalette(&ok, editPalette,
						     backgroundMode(), this);
    if (! ok)
	return;

    editPalette = pal;
    setPreviewPalette(editPalette);
    setModified(true);
}


void MainWindow::paletteSelected(int)
{
    setPreviewPalette(editPalette);
}


void MainWindow::styleSelected(const TQString &stylename)
{
    TQStyle *style = TQStyleFactory::create(stylename);
    if (! style)
	return;

    setStyleHelper(previewFrame, style);
    delete previewstyle;
    previewstyle = style;

    setModified(true);
}


void MainWindow::familySelected(const TQString &family)
{
    TQFontDatabase db;
    TQStringList styles = db.styles(family);
    stylecombo->clear();
    stylecombo->insertStringList(styles);
    familysubcombo->insertItem(family);
    buildFont();
}


void MainWindow::buildFont()
{
    TQFontDatabase db;
    TQFont font = db.font(familycombo->currentText(),
			 stylecombo->currentText(),
			 psizecombo->currentText().toInt());
    samplelineedit->setFont(font);
    setModified(true);
}


void MainWindow::substituteSelected(const TQString &family)
{
    TQStringList subs = TQFont::substitutes(family);
    sublistbox->clear();
    sublistbox->insertStringList(subs);
}


void MainWindow::removeSubstitute()
{
    if (sublistbox->currentItem() < 0 ||
	uint(sublistbox->currentItem()) > sublistbox->count())
	return;

    int item = sublistbox->currentItem();
    TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
    subs.remove(subs.at(sublistbox->currentItem()));
    sublistbox->clear();
    sublistbox->insertStringList(subs);
    if (uint(item) > sublistbox->count())
	item = int(sublistbox->count()) - 1;
    sublistbox->setCurrentItem(item);
    TQFont::removeSubstitution(familysubcombo->currentText());
    TQFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}


void MainWindow::addSubstitute()
{
    if (sublistbox->currentItem() < 0 ||
	uint(sublistbox->currentItem()) > sublistbox->count()) {
	TQFont::insertSubstitution(familysubcombo->currentText(), choosesubcombo->currentText());
        TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
	sublistbox->clear();
	sublistbox->insertStringList(subs);
	setModified(true);
	return;
    }

    int item = sublistbox->currentItem();
    TQFont::insertSubstitution(familysubcombo->currentText(), choosesubcombo->currentText());
    TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
    sublistbox->clear();
    sublistbox->insertStringList(subs);
    sublistbox->setCurrentItem(item);
    setModified(true);
}


void MainWindow::downSubstitute()
{
    if (sublistbox->currentItem() < 0 ||
	uint(sublistbox->currentItem()) >= sublistbox->count())
	return;

    int item = sublistbox->currentItem();
    TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
    TQStringList::Iterator it = subs.at(item);
    TQString fam = *it;
    subs.remove(it);
    it = subs.at(item);
    subs.insert(++it, fam);
    sublistbox->clear();
    sublistbox->insertStringList(subs);
    sublistbox->setCurrentItem(item + 1);
    TQFont::removeSubstitution(familysubcombo->currentText());
    TQFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}


void MainWindow::upSubstitute()
{
    if (sublistbox->currentItem() < 1)
	return;

    int item = sublistbox->currentItem();
    TQStringList subs = TQFont::substitutes(familysubcombo->currentText());
    TQStringList::Iterator it = subs.at(item);
    TQString fam = *it;
    subs.remove(it);
    it = subs.at(item - 1);
    subs.insert(it, fam);
    sublistbox->clear();
    sublistbox->insertStringList(subs);
    sublistbox->setCurrentItem(item - 1);
    TQFont::removeSubstitution(familysubcombo->currentText());
    TQFont::insertSubstitutions(familysubcombo->currentText(), subs);
    setModified(true);
}


void MainWindow::removeLibpath()
{
    if (libpathlistbox->currentItem() < 0 ||
	uint(libpathlistbox->currentItem()) > libpathlistbox->count())
	return;

    int item = libpathlistbox->currentItem();
    TQStringList paths = TQApplication::libraryPaths();
    paths.remove(paths.at(libpathlistbox->currentItem()));
    libpathlistbox->clear();
    libpathlistbox->insertStringList(paths);
    if (uint(item) > libpathlistbox->count())
	item = int(libpathlistbox->count()) - 1;
    libpathlistbox->setCurrentItem(item);
    TQApplication::setLibraryPaths(paths);
    setModified(true);
}


void MainWindow::addLibpath()
{
    if (libpathlineedit->text().isEmpty())
	return;

    if (libpathlistbox->currentItem() < 0 ||
	uint(libpathlistbox->currentItem()) > libpathlistbox->count()) {
	TQStringList paths = TQApplication::libraryPaths();
	paths.append(libpathlineedit->text());
	libpathlistbox->clear();
	libpathlistbox->insertStringList(paths);
	TQApplication::setLibraryPaths(paths);
	setModified(true);

	return;
    }

    int item = libpathlistbox->currentItem();
    TQStringList paths =TQApplication::libraryPaths();
    paths.insert(++paths.at(libpathlistbox->currentItem()),
		 libpathlineedit->text());
    libpathlistbox->clear();
    libpathlistbox->insertStringList(paths);
    libpathlistbox->setCurrentItem(item);
    TQApplication::setLibraryPaths(paths);
    setModified(true);
}


void MainWindow::downLibpath()
{
    if (libpathlistbox->currentItem() < 0 ||
	uint(libpathlistbox->currentItem()) >= libpathlistbox->count() - 1)
	return;

    int item = libpathlistbox->currentItem();
    TQStringList paths = TQApplication::libraryPaths();
    TQStringList::Iterator it = paths.at(item);
    TQString fam = *it;
    paths.remove(it);
    it = paths.at(item);
    paths.insert(++it, fam);
    libpathlistbox->clear();
    libpathlistbox->insertStringList(paths);
    libpathlistbox->setCurrentItem(item + 1);
    TQApplication::setLibraryPaths(paths);
    setModified(true);
}


void MainWindow::upLibpath()
{
    if (libpathlistbox->currentItem() < 1)
	return;

    int item = libpathlistbox->currentItem();
    TQStringList paths = TQApplication::libraryPaths();
    TQStringList::Iterator it = paths.at(item);
    TQString fam = *it;
    paths.remove(it);
    it = paths.at(item - 1);
    paths.insert(it, fam);
    libpathlistbox->clear();
    libpathlistbox->insertStringList(paths);
    libpathlistbox->setCurrentItem(item - 1);
    TQApplication::setLibraryPaths(paths);
    setModified(true);
}


void MainWindow::browseLibpath()
{
    TQString dirname = TQFileDialog::getExistingDirectory(TQString::null, this, 0,
							tr("Select a Directory"));
    if (dirname.isNull())
	return;

    libpathlineedit->setText(dirname);
}


void MainWindow::removeFontpath()
{
    if (fontpathlistbox->currentItem() < 0 ||
	uint(fontpathlistbox->currentItem()) > fontpathlistbox->count())
	return;

    int item = fontpathlistbox->currentItem();
    fontpaths.remove(fontpaths.at(fontpathlistbox->currentItem()));
    fontpathlistbox->clear();
    fontpathlistbox->insertStringList(fontpaths);
    if (uint(item) > fontpathlistbox->count())
	item = int(fontpathlistbox->count()) - 1;
    fontpathlistbox->setCurrentItem(item);
    setModified(true);
}


void MainWindow::addFontpath()
{
    if (fontpathlineedit->text().isEmpty())
	return;

    if (fontpathlistbox->currentItem() < 0 ||
	uint(fontpathlistbox->currentItem()) > fontpathlistbox->count()) {
	fontpaths.append(fontpathlineedit->text());
	fontpathlistbox->clear();
	fontpathlistbox->insertStringList(fontpaths);
	setModified(true);

	return;
    }

    int item = fontpathlistbox->currentItem();
    fontpaths.insert(++fontpaths.at(fontpathlistbox->currentItem()),
		     fontpathlineedit->text());
    fontpathlistbox->clear();
    fontpathlistbox->insertStringList(fontpaths);
    fontpathlistbox->setCurrentItem(item);
    setModified(true);
}


void MainWindow::downFontpath()
{
    if (fontpathlistbox->currentItem() < 0 ||
	uint(fontpathlistbox->currentItem()) >= fontpathlistbox->count() - 1)
	return;

    int item = fontpathlistbox->currentItem();
    TQStringList::Iterator it = fontpaths.at(item);
    TQString fam = *it;
    fontpaths.remove(it);
    it = fontpaths.at(item);
    fontpaths.insert(++it, fam);
    fontpathlistbox->clear();
    fontpathlistbox->insertStringList(fontpaths);
    fontpathlistbox->setCurrentItem(item + 1);
    setModified(true);
}


void MainWindow::upFontpath()
{
    if (fontpathlistbox->currentItem() < 1)
	return;

    int item = fontpathlistbox->currentItem();
    TQStringList::Iterator it = fontpaths.at(item);
    TQString fam = *it;
    fontpaths.remove(it);
    it = fontpaths.at(item - 1);
    fontpaths.insert(it, fam);
    fontpathlistbox->clear();
    fontpathlistbox->insertStringList(fontpaths);
    fontpathlistbox->setCurrentItem(item - 1);
    setModified(true);
}


void MainWindow::browseFontpath()
{
    TQString dirname = TQFileDialog::getExistingDirectory(TQString::null, this, 0,
							tr("Select a Directory"));
    if (dirname.isNull())
	return;

   fontpathlineedit->setText(dirname);
}


void MainWindow::somethingModified()
{
    setModified(true);
}


void MainWindow::helpAbout()
{
    TQMessageBox::about(this, tr("TQt Configuration"),
		       tr(about_text));
}


void MainWindow::helpAboutTQt()
{
    TQMessageBox::aboutTQt(this, tr("TQt Configuration"));
}


void MainWindow::pageChanged(TQWidget *page)
{
    if (page == tab)
	helpview->setText(tr(appearance_text));
    else if (page == tab_2)
	helpview->setText(tr(font_text));
    else if (page == tab_3)
	helpview->setText(tr(interface_text));
    else if (page == tab_4)
	helpview->setText(tr(libpath_text));
    else if (page == tab_5)
	helpview->setText(tr(printer_text));
}


void MainWindow::closeEvent(TQCloseEvent *e)
{
    if (modified) {
	switch(TQMessageBox::warning(this, tr("Save Changes"),
				    tr("Save changes to settings?"),
				    tr("&Yes"), tr("&No"), tr("&Cancel"), 0, 2)) {
	case 0: // save
	    tqApp->processEvents();
	    fileSave();

	    // fall through intended
	case 1: // don't save
	    e->accept();
	    break;

	case 2: // cancel
	    e->ignore();
	    break;

	default: break;
	}
    } else
	e->accept();
}
