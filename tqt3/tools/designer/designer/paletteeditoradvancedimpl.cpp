/**********************************************************************
** Copyright (C) 2005-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of TQt Designer.
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

#include "paletteeditoradvancedimpl.h"
#include "styledbutton.h"
#include "mainwindow.h"
#include "formwindow.h"

#include <ntqcheckbox.h>
#include <ntqcombobox.h>
#include <ntqapplication.h>
#include <ntqpushbutton.h>
#include <ntqlistbox.h>
#include <ntqpainter.h>
#include <ntqgroupbox.h>

/*!
    Class used by PaletteEditor for bold combobox items
*/

class BoldListBoxText : public TQListBoxText
{
public:
    BoldListBoxText( TQString text, TQListBox* lb = 0 );

protected:
    virtual void paint( TQPainter* );
};

BoldListBoxText::BoldListBoxText( TQString text, TQListBox* lb )
    : TQListBoxText( lb )
{
    setText( text );
}

void BoldListBoxText::paint( TQPainter* painter )
{
    TQFont f = painter->font();
    f.setBold( true );
    painter->setFont( f );

    TQListBoxText::paint( painter );
}

PaletteEditorAdvanced::PaletteEditorAdvanced( FormWindow *fw, TQWidget * parent,
					      const char * name, bool modal, WFlags f )
    : PaletteEditorAdvancedBase( parent, name, modal, f ), formWindow( fw ), selectedPalette(0)
{
    connect( buttonHelp, TQ_SIGNAL( clicked() ), MainWindow::self, TQ_SLOT( showDialogHelp() ) );

    buttonPixmap->setEditor( StyledButton::PixmapEditor );

    editPalette = TQApplication::palette();
    setPreviewPalette( editPalette );

    buttonPixmap->setFormWindow( formWindow );

    // force toggle event
    checkBuildEffect->setChecked(false);
    checkBuildEffect->setChecked(true);
}

PaletteEditorAdvanced::~PaletteEditorAdvanced()
{
}

void PaletteEditorAdvanced::onToggleBuildInactive( bool v )
{
    if (selectedPalette == 1) {
	groupCentral->setDisabled(v);
	groupEffect->setDisabled(v);
    }

    if (v) {
	buildInactive();
	updateStyledButtons();
    }
}

void PaletteEditorAdvanced::onToggleBuildDisabled( bool v )
{
    if (selectedPalette == 2) {
	groupCentral->setDisabled(v);
	groupEffect->setDisabled(v);
    }

    if (v) {
	buildDisabled();
	updateStyledButtons();
    }
}

void PaletteEditorAdvanced::paletteSelected(int p)
{
    selectedPalette = p;

    if(p == 1) { // inactive
	groupCentral->setDisabled(checkBuildInactive->isChecked());
	groupEffect->setDisabled(checkBuildInactive->isChecked());
    }
    else if (p == 2) { // disabled
	groupCentral->setDisabled(checkBuildDisabled->isChecked());
	groupEffect->setDisabled(checkBuildDisabled->isChecked());
    }
    else {
	groupCentral->setEnabled(true);
	groupEffect->setEnabled(true);
    }
    updateStyledButtons();
}

void PaletteEditorAdvanced::onChooseCentralColor()
{
    switch(selectedPalette) {
    case 0:
    default:
	mapToActiveCentralRole( buttonCentral->color() );
	break;
    case 1:
	mapToInactiveCentralRole( buttonCentral->color() );
	break;
    case 2:
	mapToDisabledCentralRole( buttonCentral->color() );
	break;
    }
    updateStyledButtons();
}

void PaletteEditorAdvanced::onChooseEffectColor()
{
    switch(selectedPalette) {
    case 0:
    default:
	mapToActiveEffectRole( buttonEffect->color() );
	break;
    case 1:
	mapToInactiveEffectRole( buttonEffect->color() );
	break;
    case 2:
	mapToDisabledEffectRole( buttonEffect->color() );
	break;
    }
    updateStyledButtons();
}

void PaletteEditorAdvanced::onChoosePixmap()
{
    if (buttonPixmap->pixmap()) {
	switch(selectedPalette) {
	case 0:
	default:
	    mapToActivePixmapRole( *buttonPixmap->pixmap() );
	    break;
	case 1:
	    mapToInactivePixmapRole( *buttonPixmap->pixmap() );
	    break;
	case 2:
	    mapToDisabledPixmapRole( *buttonPixmap->pixmap() );
	    break;
	}
    }
    updateStyledButtons();
}

void PaletteEditorAdvanced::onToggleBuildEffects( bool on )
{
    if (!on) return;
    buildActiveEffect();
    buildInactiveEffect();
    buildDisabledEffect();
}

TQColorGroup::ColorRole PaletteEditorAdvanced::centralFromItem( int item )
{
    switch( item ) {
        case 0:
            return TQColorGroup::Background;
        case 1:
            return TQColorGroup::Foreground;
        case 2:
            return TQColorGroup::Button;
        case 3:
            return TQColorGroup::Base;
        case 4:
            return TQColorGroup::Text;
        case 5:
            return TQColorGroup::BrightText;
        case 6:
            return TQColorGroup::ButtonText;
        case 7:
            return TQColorGroup::Highlight;
        case 8:
            return TQColorGroup::HighlightedText;
        case 9:
            return TQColorGroup::Link;
        case 10:
            return TQColorGroup::LinkVisited;
        default:
            return TQColorGroup::NColorRoles;
    }
}

TQColorGroup::ColorRole PaletteEditorAdvanced::effectFromItem( int item )
{
    switch( item ) {
    case 0:
	return TQColorGroup::Light;
    case 1:
	return TQColorGroup::Midlight;
    case 2:
	return TQColorGroup::Mid;
    case 3:
	return TQColorGroup::Dark;
    case 4:
	return TQColorGroup::Shadow;
    default:
	return TQColorGroup::NColorRoles;
    }
}

void PaletteEditorAdvanced::onCentral( int item )
{
    TQPixmap* p = 0;
    TQColor c;

    switch(selectedPalette) {
    case 0:
    default:
	c = editPalette.active().color( centralFromItem(item) );
	p = editPalette.active().brush( centralFromItem(item) ).pixmap();
	break;
    case 1:
	c = editPalette.inactive().color( centralFromItem(item) );
	p = editPalette.inactive().brush( centralFromItem(item) ).pixmap();
	break;
    case 2:
	c =  editPalette.disabled().color( centralFromItem(item) );
	p = editPalette.disabled().brush( centralFromItem(item) ).pixmap();
	break;
    }

    buttonCentral->setColor(c);

    if (p)
	buttonPixmap->setPixmap( *p );
    else
	buttonPixmap->setPixmap( TQPixmap() );
}

void PaletteEditorAdvanced::onEffect( int item )
{
    TQColor c;
    switch(selectedPalette) {
    case 0:
    default:
	c = editPalette.active().color( effectFromItem(item) );
	break;
    case 1:
	editPalette.inactive().color( effectFromItem(item) );
	break;
    case 2:
	editPalette.disabled().color( effectFromItem(item) );
	break;
    }
    buttonEffect->setColor(c);
}

void PaletteEditorAdvanced::mapToActiveCentralRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.active();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setActive( cg );

    buildActiveEffect();
    if(checkBuildInactive->isChecked())
	buildInactive();
    if(checkBuildDisabled->isChecked())
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToActiveEffectRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.active();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setActive( cg );

    if(checkBuildInactive->isChecked())
	buildInactive();
    if(checkBuildDisabled->isChecked())
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToActivePixmapRole( const TQPixmap& pm )
{
    TQColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    TQColorGroup cg = editPalette.active();
    if (  !pm.isNull()  )
	cg.setBrush( role, TQBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, TQBrush( cg.color( role ) ) );
    editPalette.setActive( cg );


    buildActiveEffect();
    if(checkBuildInactive->isChecked())
	buildInactive();
    if(checkBuildDisabled->isChecked())
	buildDisabled();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactiveCentralRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.inactive();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setInactive( cg );

    buildInactiveEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactiveEffectRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.inactive();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToInactivePixmapRole( const TQPixmap& pm )
{
    TQColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    TQColorGroup cg = editPalette.inactive();
    if (  !pm.isNull()  )
	cg.setBrush( role, TQBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, TQBrush( cg.color( role ) ) );
    editPalette.setInactive( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledCentralRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.disabled();
    cg.setColor( centralFromItem(comboCentral->currentItem()), c );
    editPalette.setDisabled( cg );

    buildDisabledEffect();

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledEffectRole( const TQColor& c )
{
    TQColorGroup cg = editPalette.disabled();
    cg.setColor( effectFromItem(comboEffect->currentItem()), c );
    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::mapToDisabledPixmapRole( const TQPixmap& pm )
{
    TQColorGroup::ColorRole role = centralFromItem(comboCentral->currentItem());
    TQColorGroup cg = editPalette.disabled();
    if (  !pm.isNull()  )
	cg.setBrush( role, TQBrush( cg.color( role ), pm ) );
    else
	cg.setBrush( role, TQBrush( cg.color( role ) ) );

    editPalette.setDisabled( cg );

    setPreviewPalette( editPalette );
}

void PaletteEditorAdvanced::buildActiveEffect()
{
    TQColorGroup cg = editPalette.active();
    TQColor btn = cg.color( TQColorGroup::Button );

    TQPalette temp( btn, btn );

    for (int i = 0; i<5; i++)
	cg.setColor( effectFromItem(i), temp.active().color( effectFromItem(i) ) );

    editPalette.setActive( cg );
    setPreviewPalette( editPalette );

    updateStyledButtons();
}

void PaletteEditorAdvanced::buildInactive()
{
    editPalette.setInactive( editPalette.active() );
    buildInactiveEffect();
}

void PaletteEditorAdvanced::buildInactiveEffect()
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
    updateStyledButtons();
}

void PaletteEditorAdvanced::buildDisabled()
{
    TQColorGroup cg = editPalette.active();
    cg.setColor( TQColorGroup::ButtonText, darkGray );
    cg.setColor( TQColorGroup::Foreground, darkGray );
    editPalette.setDisabled( cg );

    buildDisabledEffect();
}

void PaletteEditorAdvanced::buildDisabledEffect()
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
    updateStyledButtons();
}

void PaletteEditorAdvanced::setPreviewPalette( const TQPalette& pal )
{
    TQColorGroup cg;

    switch (selectedPalette) {
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
}

void PaletteEditorAdvanced::updateStyledButtons()
{
    TQColor central, effect;
    TQPixmap *pm = 0;
    switch (selectedPalette) {
    case 0:
    default:
	central = editPalette.active().color( centralFromItem( comboCentral->currentItem() ) );
	effect = editPalette.active().color( effectFromItem( comboEffect->currentItem() ) );
	pm = editPalette.active().brush( centralFromItem( comboCentral->currentItem() ) ).pixmap();
	break;
    case 1:
	central = editPalette.inactive().color( centralFromItem( comboCentral->currentItem() ) );
	effect = editPalette.inactive().color( effectFromItem( comboEffect->currentItem() ) );
	pm = editPalette.inactive().brush( centralFromItem( comboCentral->currentItem() ) ).pixmap();
	break;
    case 2:
	central = editPalette.disabled().color( centralFromItem( comboCentral->currentItem() ) );
	effect = editPalette.disabled().color( effectFromItem( comboEffect->currentItem() ) );
	pm = editPalette.disabled().brush( centralFromItem( comboCentral->currentItem() ) ).pixmap();
	break;
    }

    buttonCentral->setColor(central);
    buttonEffect->setColor(effect);
    if ( pm && !pm->isNull() )
	buttonPixmap->setPixmap( *pm );
    else
	buttonPixmap->setPixmap(TQPixmap());
}

void PaletteEditorAdvanced::setPal( const TQPalette& pal )
{
    editPalette = pal;
    setPreviewPalette( pal );
    updateStyledButtons();
}

TQPalette PaletteEditorAdvanced::pal() const
{
    return editPalette;
}

void PaletteEditorAdvanced::setupBackgroundMode( BackgroundMode mode )
{
    int initRole = 0;

    switch( mode ) {
    case PaletteBackground:
	initRole = 0;
	break;
    case PaletteForeground:
	initRole = 1;
	break;
    case PaletteButton:
	initRole = 2;
	break;
    case PaletteBase:
	initRole = 3;
	break;
    case PaletteText:
	initRole = 4;
	break;
    case PaletteBrightText:
	initRole = 5;
	break;
    case PaletteButtonText:
	initRole = 6;
	break;
    case PaletteHighlight:
	initRole = 7;
	break;
    case PaletteHighlightedText:
	initRole = 8;
	break;
    case PaletteLight:
	initRole = 9;
	break;
    case PaletteMidlight:
	initRole = 10;
	break;
    case PaletteDark:
	initRole = 11;
	break;
    case PaletteMid:
	initRole = 12;
	break;
    case PaletteShadow:
	initRole = 13;
	break;
    default:
	initRole = -1;
	break;
    }

    if ( initRole <= -1 ) return;

    if (initRole > 8 ) {
	comboEffect->setCurrentItem( initRole - 9 );
	if ( comboEffect->listBox() ) {
	    TQString text = comboEffect->currentText();
	    comboEffect->listBox()->changeItem( new BoldListBoxText( text ), initRole - 9 );
	}
    }
    else {
	comboCentral->setCurrentItem( initRole );
	if ( comboCentral->listBox() ) {
	    TQString text = comboCentral->currentText();
	    comboCentral->listBox()->changeItem( new BoldListBoxText( text ), initRole );
	}
    }
}

TQPalette PaletteEditorAdvanced::getPalette( bool *ok, const TQPalette &init,
					    BackgroundMode mode, TQWidget* parent,
					    const char* name, FormWindow *fw )
{
    PaletteEditorAdvanced* dlg = new PaletteEditorAdvanced( fw, parent, name, true );
    dlg->setupBackgroundMode( mode );

    if ( init != TQPalette() )
        dlg->setPal( init );
    int resultCode = dlg->exec();

    TQPalette result = init;
    if ( resultCode == TQDialog::Accepted ) {
	if ( ok )
	    *ok = true;
	result = dlg->pal();
    } else {
	if ( ok )
	    *ok = false;
    }
    delete dlg;
    return result;
}
